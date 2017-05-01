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

#ifndef HID_INPUT_DRIVER_H
#define HID_INPUT_DRIVER_H

#include <Windows.h>

namespace ecu {
namespace hid {

class InputDriver {
 public:
  bool Register(HWND window);
 private:
};

}  // namespace hid
}  // namespace ecu

#endif
