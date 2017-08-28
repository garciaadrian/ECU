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

#ifndef HID_INPUT_SYSTEM_H_
#define HID_INPUT_SYSTEM_H_

#include <vector>
#include <memory>
#include <unordered_map>

#include "base/console_command.h"
#include "base/console_system.h"
#include "hid/input_command.h"
#include "hid/input_driver.h"
#include "ui/ui_event.h"

namespace ecu {
namespace hid {

class InputSystem {
 public:
  explicit InputSystem(ecu::ConsoleSystem* console_system);
  ~InputSystem();
  
  void AddDriver(std::unique_ptr<InputDriver> driver);
  void AddCommand(std::unique_ptr<InputCommand> command);

  int GetState(ecu::ui::RawInputEvent* e);

  std::pair<int, int> GetPacketId(HANDLE device);

  void Bind(uint16_t key, const std::string& command_name);
  
  std::vector<std::unique_ptr<ConCommand>> CreateConsoleCommands();

 private:
  ecu::ui::Window* window_ = nullptr;
  ecu::ConsoleSystem* console_system_ = nullptr;
  std::vector<std::unique_ptr<InputDriver>> drivers_;
  std::vector<std::unique_ptr<InputCommand>> commands_;
  std::unordered_map<uint16_t, InputCommand*> bind_map_;

};

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window);

void MoveInputCommands(std::vector<std::unique_ptr<ecu::hid::InputCommand>> to,
                       std::vector<std::unique_ptr<ecu::hid::InputCommand>> from);

void MoveInputCommands(ecu::hid::InputSystem* input_system,
                       std::vector<std::unique_ptr<ecu::hid::InputCommand>>& commands);

}  // namespace hid
}  // namespace ecu

#endif  // HID_INPUT_SYSTEM_H_
