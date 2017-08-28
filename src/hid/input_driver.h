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

#ifndef HID_INPUT_DRIVER_H_
#define HID_INPUT_DRIVER_H_

#include <Windows.h>
#include <map>

#include "ui/window.h"
#include "ui/ui_event.h"
#include "hid/input_command.h"

namespace ecu {
namespace hid {

class InputSystem;

class InputDriver {
 public:
  InputDriver(ecu::ui::Window* window);
  virtual ~InputDriver();

  virtual bool Register(HWND window) = 0;
  virtual int GetState(ecu::ui::RawInputEvent* e) = 0;
  virtual std::pair<int, int> GetDeviceId() = 0;

 private:
  ecu::ui::Window* window_ = nullptr;  
};

}  // namespace hid
}  // namespace ecu

#endif
