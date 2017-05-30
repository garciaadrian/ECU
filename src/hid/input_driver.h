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

#ifndef HID_INPUT_DRIVER_H_
#define HID_INPUT_DRIVER_H_

#include <Windows.h>

#include "ui/window.h"

namespace ecu {
namespace hid {

class InputDriver {
 public:
  InputDriver(ecu::ui::Window* window);
  virtual ~InputDriver();

  virtual bool Register(HWND window) = 0;
  virtual int GetState() = 0;

 private:
  ecu::ui::Window* window_ = nullptr;
};

enum MP430_INPUT_BUTTON {
  MP430_INPUT_BRAKE_BALANCE_UP = 0x001,
  MP430_INPUT_BRAKE_BALANCE_DOWN = 0x002,
};

}  // namespace hid
}  // namespace ecu

#endif
