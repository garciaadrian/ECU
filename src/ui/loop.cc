/**
*******************************************************************************
*                                                                             *
* ECU: iRacing MP4-30 Performance Analysis Project                            *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#include "ui/loop.h"
#include "ui/ui_event.h"
#include "base/delegate.h"
#include "base/threading.h"

namespace ecu {
namespace ui {

// TODO(garciaadrian): There isn't any polymorphism going on so we don't
// need a Create function like xenia does
std::unique_ptr<Loop> Loop::Create() { return std::make_unique<Loop>(); }

Loop::Loop() : thread_id_(0) {
  timer_queue_ = CreateTimerQueue();

  ecu::threading::Fence init_fence;
  thread_ = std::thread([&init_fence, this]() {
    ecu::threading::set_name("Win32 Loop");
    thread_id_ = GetCurrentThreadId();

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    init_fence.Signal();
    ThreadMain();

    quit_fence_.Signal();
  });
  init_fence.Wait();
}

Loop::~Loop() {
  Quit();
  thread_.join();

  DeleteTimerQueueEx(timer_queue_, INVALID_HANDLE_VALUE);
  std::lock_guard<std::mutex> lock(pending_timers_mutex_);
  while (!pending_timers_.empty()) {
    auto timer = pending_timers_.back();
    pending_timers_.pop_back();
    delete timer;
  }
}

bool Loop::is_on_loop_thread() {
  return thread_id_ == GetCurrentThreadId();
}

void Loop::PostSynchronous(std::function<void()> fn) {
  if (is_on_loop_thread()) {
    // Prevent deadlock if we are executing on ourselves.
    fn();
    return;
  }

  ecu::threading::Fence fence;
  Post([&fn, &fence]() {
      fn();
      fence.Signal();
    });
  fence.Wait();
}

void Loop::ThreadMain() {
  MSG msg;
  while (!should_exit_ && GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    // Process queued functions.
    std::lock_guard<std::recursive_mutex> lock(posted_functions_mutex_);
    for (auto it = posted_functions_.begin(); it != posted_functions_.end();) {
      (*it).Call();
      it = posted_functions_.erase(it);
    }
  }

  UIEvent e(nullptr);
  on_quit(&e);
}

void Loop::Post(std::function<void()> fn) {
  std::lock_guard<std::recursive_mutex> lock(posted_functions_mutex_);
  PostedFn posted_fn(fn);
  posted_functions_.push_back(posted_fn);

  while (!PostThreadMessage(thread_id_, WM_NULL, 0, 0)) {
    Sleep(1);
  }
}

void Loop::Quit() {
  _ASSERT(thread_id_ != 0);
  should_exit_ = true;
  // TODO(garciaadrian): main thread would hang here because
  // loop thread doesn't exist anymore. Xenia doesn't
  // check for GetLastError.
  while (!PostThreadMessage(thread_id_, WM_NULL, 0, 0) &&
         GetLastError() != ERROR_INVALID_THREAD_ID) {
    Sleep(1);
  }
}

void Loop::AwaitQuit() { quit_fence_.Wait(); }

}  // namespace ui
}  // namespace ecu
