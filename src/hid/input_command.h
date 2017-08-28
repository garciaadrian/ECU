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
  InputCommand();
  virtual ~InputCommand();
  
  virtual void execute() = 0;

  std::string name() { return name_; }

 protected:
  std::string name_;
  std::string description_;

};

class NullInputCommand : public InputCommand {
 public:
  NullInputCommand() {
    name_ = "null";
    description_ = "placeholder for empty command";
  }
  ~NullInputCommand() {}
  
  void execute() final override {}
};

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands();


}  // namespace hid
}  // namespace ecu

#endif  // HID_INPUT_COMMANDS_H_
