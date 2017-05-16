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

#include "ui/window.h"

namespace ecu {
namespace hid {

class InputDriver {
 public:
  InputDriver(ecu::ui::Window* window);
  virtual ~InputDriver();

  virtual bool Register(HWND window) = 0;

 private:
  ecu::ui::Window* window_ = nullptr;
};

}  // namespace hid
}  // namespace ecu

#endif
