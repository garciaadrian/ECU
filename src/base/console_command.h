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

#ifndef BASE_CONSOLE_COMMAND_H_
#define BASE_CONSOLE_COMMAND_H_

#include <vector>
#include <memory>

namespace ecu {

class ConCommand {
 public:
  ConCommand();
  virtual ~ConCommand();

  virtual void execute(const std::vector<std::string>& params) = 0;

  std::string name() const { return name_; }
  std::string description() const { return description_; }

 protected:
  std::string name_;
  std::string description_;
};

class ConCommandSystem {
 public:
  ConCommandSystem();
  ~ConCommandSystem();

  void AddCommand(std::unique_ptr<ConCommand> command);
  void ExecCommand(const std::string& name,
                   const std::vector<std::string>& params);
  ConCommand* GetCommand(const std::string& name);

 private:
  std::vector<std::unique_ptr<ConCommand>> commands_;
};

void MoveConsoleCommands(std::vector<std::unique_ptr<ecu::ConCommand>> to,
                         std::vector<std::unique_ptr<ecu::ConCommand>> from);

void MoveConsoleCommands(ecu::ConCommandSystem* command_system,
                         std::vector<std::unique_ptr<ecu::ConCommand>>& commands);

}  // namespace ecu

#endif // BASE_CONSOLE_COMMAND_H_
