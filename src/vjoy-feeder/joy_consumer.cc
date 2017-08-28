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

#include "vjoy-feeder/joy_consumer.h"

#include "vJoydriver/inc/vjoyinterface.h"

namespace ecu {
namespace vjoy {

JoyConsumer::JoyConsumer() {
  thread_ = std::thread(&JoyConsumer::ThreadMain, this);
}

JoyConsumer::~JoyConsumer() {
  should_exit_ = true;
  thread_.join();
}

void JoyConsumer::Quit() {
  should_exit_ = true;
}

void JoyConsumer::AwaitQuit() {
  quit_fence_.Wait();
}

void JoyConsumer::PostPosition(const JOYSTICK_POSITION_V2 position) {

  std::unique_lock<std::mutex> lock(queue_mutex_);
  position_queue_.push(position);
}

void JoyConsumer::ThreadMain() {
  ecu::threading::set_name("Joy Consumer Thread");
  bool acquired = false;
  
  while (!should_exit_) {
    acquired = false;
    
    queue_mutex_.lock();
    if (!position_queue_.empty()) {
      auto& position = position_queue_.front();
      position_queue_.pop();

      acquired = true;
      
      JOYSTICK_POSITION_V2 reset = {0};
      auto updated = UpdateVJD(position.bDevice, &reset);
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
      
      updated = UpdateVJD(position.bDevice, &position);
      std::this_thread::sleep_for(std::chrono::milliseconds(16));

      updated = UpdateVJD(position.bDevice, &reset);
    }
    queue_mutex_.unlock();
    if (!acquired) {
      std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
  }
  quit_fence_.Signal();
}

}  // namespace vjoy
}  // namespace ecu

