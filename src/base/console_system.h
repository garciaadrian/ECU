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

#ifndef BASE_CONSOLE_H_
#define BASE_CONSOLE_H_

#include <memory>

#include "base/cvars.h"
#include "base/console_command.h"

namespace ecu {

class ConsoleSystem {
 public:
  ConsoleSystem();
  ~ConsoleSystem();

  CVarSystem* cvar_system() const {
    return cvar_system_.get();
  }
  
  ConCommandSystem* command_system() const {
    return command_system_.get();
  }

  // Only run after all systems have loaded their commands/cvars
  void ParseAutoExec(const std::string& filename);

 private:
  std::unique_ptr<CVarSystem> cvar_system_;
  std::unique_ptr<ConCommandSystem> command_system_;
};

}  // namespace ecu

#endif // BASE_CONSOLE_H_
