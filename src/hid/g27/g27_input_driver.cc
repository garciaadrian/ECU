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

#include "hid/g27/g27_input_driver.h"

#include <Windows.h>

namespace ecu {
namespace hid {
namespace g27 {

G27InputDriver::G27InputDriver(ecu::ui::Window* window) : InputDriver(window) {
  bool ret = Register(window->hwnd());
}

G27InputDriver::~G27InputDriver() { RAWINPUTDEVICE wheel_input = {0}; }

bool G27InputDriver::Register(HWND window) {
  RAWINPUTDEVICE wheel_input = {0};
  wheel_input.usUsagePage = 1;
  wheel_input.usUsage = 4;
  wheel_input.dwFlags = RIDEV_INPUTSINK;
  wheel_input.hwndTarget = window;

  if (!RegisterRawInputDevices(&wheel_input, 1, sizeof(RAWINPUTDEVICE))) {
    return false;
  }
  return true;
}

}  // namespace g27
}  // namespace hid
}  // namespace ecu
