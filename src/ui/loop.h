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

#ifndef UI_LOOP_H_
#define UI_LOOP_H_

#include <functional>
#include <thread>
#include <windows.h>
#include <mutex>
#include <list>

#include "base/threading.h"
#include "base/delegate.h"
#include "ui/ui_event.h"

namespace ecu {
namespace ui {

class Loop {
 public:
  Loop();
  ~Loop();
  
  std::unique_ptr<Loop> Create();
  bool is_on_loop_thread();

  void Post(std::function<void()> fn);
  void PostSynchronous(std::function<void()> fn);

  void ThreadMain();

  void Quit();
  void AwaitQuit();

  Delegate<UIEvent*> on_quit;

 private:
  struct PendingTimer {
    Loop* loop;
    HANDLE timer_queue;
    HANDLE timer_handle;
    std::function<void()> fn;
  };

  class PostedFn {
   public:
    explicit PostedFn(std::function<void()> fn) : fn_(std::move(fn)) {}
    void Call() { fn_(); }

   private:
    std::function<void()> fn_;
  };

  std::thread thread_;
  DWORD thread_id_;
  HANDLE timer_queue_;
  bool should_exit_ = false;
  ecu::threading::Fence quit_fence_;

  std::mutex pending_timers_mutex_;
  std::list<PendingTimer*> pending_timers_;
  
  std::recursive_mutex posted_functions_mutex_;
  std::list<PostedFn> posted_functions_;
};

}  // namespace ui
}  // namespace ecu

#endif
