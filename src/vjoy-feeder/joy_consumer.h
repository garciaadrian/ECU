/**
*******************************************************************************
*                                                                             *
* ECU: iRacing Extensions Collection Project                                  *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#ifndef VJOY_FEEDER_JOY_CONSUMER_H_
#define VJOY_FEEDER_JOY_CONSUMER_H_

#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <Windows.h> // this is for public.h
#include "vJoydriver/inc/public.h"

#include "base/threading.h"

namespace ecu {
namespace vjoy {

class JoyConsumer {
 public:
  JoyConsumer();
  ~JoyConsumer();

  /**
     Add a joystick position to the queue
   */
  void PostPosition(const JOYSTICK_POSITION_V2 position);

  void Quit();
  void AwaitQuit();

 private:
  JoyConsumer(const JoyConsumer&)=delete;
  JoyConsumer& operator=(const JoyConsumer&)=delete;

  std::queue<JOYSTICK_POSITION_V2> position_queue_;
  std::mutex queue_mutex_;
  std::condition_variable cv;

  void ThreadMain();
  std::thread thread_;

  std::atomic<bool> should_exit_ = false;
  ecu::threading::Fence quit_fence_;
};

}  // namespace vjoy
}  // namespace ecu

#endif // VJOY_FEEDER_JOY_CONSUMER_H_


