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

#include "hid/input_driver.h"

namespace ecu {
namespace hid {

InputDriver::InputDriver(ecu::ui::Window* window) : window_(window) {}

InputDriver::~InputDriver() = default;

}  // namespace hid
}  // namespace ecu
