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

#include "hid/input_system.h"
#include "hid/g27/g27_hid.h"

namespace ecu {
namespace hid {

void InputSystem::AddDriver(std::unique_ptr<hid::InputDriver> driver) {
  drivers_.push_back(std::move(driver));
}

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window) {
  std::vector<std::unique_ptr<hid::InputDriver>> drivers;

  drivers.emplace_back(ecu::hid::g27::Create(window));
  return drivers;
}

}  // namespace hid
}  // namespace ecu
