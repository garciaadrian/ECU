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

#include "hid/g27/g27_hid.h"
#include "hid/g27/g27_input_driver.h"

namespace ecu {
namespace hid {
namespace g27 {

std::unique_ptr<InputDriver> Create(ecu::ui::Window* window) {
  return std::make_unique<G27InputDriver>(window);
}

}  // namespace g27
}  // namespace hid
}  // namespace ecu
