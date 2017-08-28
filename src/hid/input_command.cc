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

#include "hid/input_command.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands() {
  std::vector<std::unique_ptr<hid::InputCommand>> commands;
  commands.push_back(std::make_unique<NullInputCommand>());
  return commands;
}

InputCommand::InputCommand() {}

InputCommand::~InputCommand() = default;

// void MoveInputCommands(std::vector<std::unique_ptr<ecu::hid::InputCommand>> to,
//                        std::vector<std::unique_ptr<ecu::hid::InputCommand>> from) {
//   for (auto& command : from) {
//     to.push_back(std::move(command));
//   }
// }

// void MoveInputCommands(ecu::hid::InputSystem* input_system,
//                        std::vector<std::unique_ptr<ecu::hid::InputCommand>>& commands) {
//   for (auto& command : commands) {
//     input_system->AddCommand(std::move(command));
//   }
// }


}  // namespace hid
}  // namespace ecu
