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

#ifndef HID_G27_G27_HID_H_
#define HID_G27_G27_HID_H_

#include <memory>

#include "hid/input_system.h"

namespace ecu {
namespace hid {
namespace g27 {

std::unique_ptr<InputDriver> Create(ecu::ui::Window* window);

}  // namespace g27
}  // namespace hid
}  // namespace ecu

#endif  // HID_G27_G27_HID_H_
