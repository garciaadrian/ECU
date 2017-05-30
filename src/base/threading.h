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

#ifndef BASE_THREADING_H_
#define BASE_THREADING_H_

#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace ecu {
namespace threading {

class Fence {
 public:
  Fence() : signaled_(false) {}
  void Signal() {
    std::unique_lock<std::mutex> lock(mutex_);
    signaled_.store(true);
    cond_.notify_all();
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!signaled_.load()) {
      cond_.wait(lock);
    }
    signaled_.store(false);
  }

 private:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::atomic<bool> signaled_;
};

void set_name(const std::string& name);

}  // namespace threading
}  // namespace ecu

#endif
