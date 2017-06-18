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

#ifndef HID_INPUT_SYSTEM_H_
#define HID_INPUT_SYSTEM_H_

#include <vector>
#include <memory>

#include "hid/input_command.h"
#include "hid/input_driver.h"
#include "ui/ui_event.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window);

class InputSystem {
 public:
  explicit InputSystem(ecu::ui::Window* window);
  ~InputSystem();
  void AddDriver(std::unique_ptr<InputDriver> driver);
  void AddCommand(std::unique_ptr<InputCommand> command);

  int GetState(ecu::ui::RawInputEvent* e);

  std::pair<int, int> GetPacketId(HANDLE device);

 private:
  ecu::ui::Window* window_;

  std::vector<std::unique_ptr<InputDriver>> drivers_;
  std::vector<std::unique_ptr<InputCommand>> commands_;
};

}  // namespace hid
}  // namespace ecu

#endif  // HID_INPUT_SYSTEM_H_
