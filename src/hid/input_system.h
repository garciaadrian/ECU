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

#include "hid/input_driver.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window);

class InputSystem {
 public:
  void AddDriver(std::unique_ptr<InputDriver> driver);

 private:
  std::vector<std::unique_ptr<InputDriver>> drivers_;
};

}  // namespace hid
}  // namespace ecu

#endif  // HID_INPUT_SYSTEM_H_
