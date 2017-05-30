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

#include "base/threading.h"

namespace ecu {
namespace ui {

Loop::Loop() : thread_id_(0) {
  timer_queue_ = CreateTimerQueue();

  ecu::threading::Fence init_fence;
  thread_ = std::thread([&init_fence, this]() {
    ecu::threading::set_name("Win32 Loop");
    thread_id_ = GetCurrentThreadId();

    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    init_fence.Signal();
  });
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
}

}  // namespace ui
}  // namespace ecu
