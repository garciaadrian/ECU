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

#include "hid/input_command.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands() {
  std::vector<std::unique_ptr<hid::InputCommand>> commands;

  return commands;
}

}  // namespace hid
}  // namespace ecu
