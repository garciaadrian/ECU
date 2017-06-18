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

#ifndef HID_INPUT_COMMANDS_H_
#define HID_INPUT_COMMANDS_H_

#include <string>
#include <vector>
#include <memory>

#include "base/command.h"

using ecu::base::Command;

namespace ecu {
namespace hid {

class InputCommand : Command {
 public:
  virtual std::string name() = 0;
  virtual void execute() = 0;
  virtual int serialize(std::ostream&) const = 0;
};

class NullInputCommand : InputCommand {
 public:
  std::string name() final override { return std::string(""); }
  void execute() final override {}
};

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands();

}  // namespace hid
}  // namespace ecu

#endif  // HID_INPUT_COMMANDS_H_
