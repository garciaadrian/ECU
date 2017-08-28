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

#include "base/console_system.h"

#include <fstream>
#include <regex>

bool is_delimiter(char value, const std::string& delims) {
  for (auto d: delims) {
    if (d == value) return true;
  }
  return false;
}

std::vector<std::string>
split(const std::string& str, const std::string& delims = " ",
      bool skip_empty = true) {
  std::vector<std::string> output;
  auto first = std::cbegin(str);

  while (first != std::cend(str)) {
    const auto second = std::find_first_of(first, std::cend(str),
                                           std::cbegin(delims), std::cend(delims));
    if (first != second || !skip_empty) {
      output.emplace_back(first, second);
    }
    if (second == std::cend(str)) break;
    first = std::next(second);
  }

  return output;
}

namespace ecu {

ConsoleSystem::ConsoleSystem() {
  cvar_system_ = std::make_unique<CVarSystem>();
  command_system_ = std::make_unique<ConCommandSystem>();
}

ConsoleSystem::~ConsoleSystem() {}

void ConsoleSystem::ParseAutoExec(const std::string& filename) {
  std::string line; 
  auto autoexec = std::ifstream(filename);
  
  while (std::getline(autoexec, line)) {
    auto tokens = split(line);
    
    // must be a command so don't bother checking if it's a cvar    
    if (tokens.size() == 1) {
      command_system_->ExecCommand(tokens[0], std::vector<std::string>());
      continue;
    }
    
    std::vector<std::string> params(tokens.begin() + 1, tokens.end());
    
    auto command = command_system_->GetCommand(tokens[0]);
    if (command != nullptr) {
      command->execute(params);
      params.clear();
    }
    
    // must be a cvar
    else if (tokens.size() == 2) {
      size_t size;
      
      cvar_system_->SetCVar(tokens[0], std::stof(tokens[1], &size));
      params.clear();
    }
  }
}


}  // namespace ecu
