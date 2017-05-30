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

#ifndef HID_G27_G27_INPUT_DRIVER_H_
#define HID_G27_G27_INPUT_DRIVER_H_

#include <Windows.h>

#include "hid/input_system.h"

namespace ecu {
namespace hid {
namespace g27 {

class G27InputDriver : public InputDriver {
 public:
  explicit G27InputDriver(ecu::ui::Window* window);
  ~G27InputDriver();

  bool Register(HWND window) override;
  int GetState() override;

 private:
  unsigned short vendor_id_ = 0x46d;
  unsigned short product_id_ = 0xc29b;
  std::string device_path;
};

}  // namespace g27
}  // namespace hid
}  // namespace ecu

#endif  // HID_G27_G27_INPUT_DRIVER_H_
