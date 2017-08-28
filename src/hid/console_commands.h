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

#ifndef HID_CONSOLE_COMMANDS_H_
#define HID_CONSOLE_COMMANDS_H_

#include "base/console_command.h"
#include "hid/input_system.h"

namespace ecu {
namespace hid {

class BindCommand : public ConCommand {
 public:
  BindCommand(InputSystem* input_system) : input_system_(input_system) {
    name_ = "bind";
    description_ = "bind an input command to a button";
  };
  ~BindCommand() {};

  void execute(const std::vector<std::string>& params) final override {
    if (params.size() != 2) {
      return;
    }

    input_system_->Bind(static_cast<uint16_t>(std::stoi(params[0])), params[1]);
  };
  
 private:
  InputSystem* input_system_;
};

}  // namespace hid
}  // namespace ecu

#endif // HID_CONSOLE_COMMANDS_H_
