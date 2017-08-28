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

#ifndef HID_G27_G27_INPUT_DRIVER_H_
#define HID_G27_G27_INPUT_DRIVER_H_

#include <Windows.h>

#include "hid/input_system.h"

namespace ecu {
namespace hid {
namespace g27 {

class G27InputDriver : public InputDriver {
 public:
  G27InputDriver(ecu::ui::Window* window);
  ~G27InputDriver();

  bool Register(HWND window) override;

  int PollState();
  int GetState(ecu::ui::RawInputEvent* e) override;

  std::pair<int, int> GetDeviceId() override;
  
 private:
  const unsigned short kVendor_id_ = 0x46d;
  const unsigned short kProduct_id_ = 0xc29b;

  std::string device_path_;
};

}  // namespace g27
}  // namespace hid
}  // namespace ecu

#endif  // HID_G27_G27_INPUT_DRIVER_H_
