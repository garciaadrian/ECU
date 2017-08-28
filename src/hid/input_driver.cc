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

#include "hid/input_driver.h"

namespace ecu {
namespace hid {

class InputSystem;

InputDriver::InputDriver(ecu::ui::Window* window)
    : window_(window) {}

InputDriver::~InputDriver() = default;

}  // namespace hid
}  // namespace ecu
