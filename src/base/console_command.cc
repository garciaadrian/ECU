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

#include "base/console_command.h"

namespace ecu {

ConCommand::ConCommand() {}
ConCommand::~ConCommand() = default;

ConCommandSystem::ConCommandSystem() {}
ConCommandSystem::~ConCommandSystem() {}

void ConCommandSystem::AddCommand(std::unique_ptr<ConCommand> command) {
  commands_.push_back(std::move(command));
}

ConCommand* ConCommandSystem::GetCommand(const std::string& name) {
  for (auto& command : commands_) {
    if (command->name() == name) {
      return command.get();
    }
  }

  return nullptr;
}

void ConCommandSystem::ExecCommand(const std::string& name,
                                   const std::vector<std::string>& params) {
  for (auto& command : commands_) {
    if (command->name() == name) {
      command->execute(params);
    }
  }
}

void MoveConsoleCommands(std::vector<std::unique_ptr<ecu::ConCommand>> to,
                         std::vector<std::unique_ptr<ecu::ConCommand>> from) {
  for (auto& command : from) {
    to.push_back(std::move(command));
  }
}

void MoveConsoleCommands(ecu::ConCommandSystem* command_system,
                         std::vector<std::unique_ptr<ecu::ConCommand>>& commands) {
  for (auto& command : commands) {
    command_system->AddCommand(std::move(command));
  }
}


}  // namespace ecu
