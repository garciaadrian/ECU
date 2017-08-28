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

#ifndef CAR_ECU_H_
#define CAR_ECU_H_

#include <thread>
#include <atomic>

#include "base/console_system.h"
#include "base/threading.h"
#include "hid/input_command.h"
#include "car/brakes/brakes.h"
#include "vjoy-feeder/joy_consumer.h"

namespace ecu {
namespace car {

class ControlUnit {
 public:
  explicit ControlUnit(ConsoleSystem* console);
  ControlUnit(const ControlUnit &) = default;
  ~ControlUnit();

  void ThreadMain();
  void Quit();
  void AwaitQuit();

  std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands();
  std::vector<std::unique_ptr<ecu::ConCommand>> CreateConsoleCommands();

 private:
  ConsoleSystem* console_;
  BrakeSystem brakes_;

  std::atomic<bool> should_exit_ = false;
  ecu::threading::Fence quit_fence_;
  std::thread thread_;
};

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands();
std::vector<std::unique_ptr<ecu::CVar>> CreateCVars();


}  // namespace car
}  // namespace ecu

#endif // CAR_ECU_H_
