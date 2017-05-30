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

enum G27_INPUT_BUTTON {
  G27_INPUT_SHIFTER_DOWN = 0x001,
  G27_INPUT_SHIFTER_UP = 0x002,
  G27_INPUT_WHEEL_BUTTON_ONE = 0x003,
  G27_INPUT_WHEEL_BUTTON_TWO = 0x004,
  G27_INPUT_WHEEL_BUTTON_THREE = 0x005,
  G27_INPUT_WHEEL_BUTTON_FOUR = 0x006,
  G27_INPUT_WHEEL_BUTTON_FIVE = 0x007,
  G27_INPUT_WHEEL_BUTTON_SIX = 0x008,
  G27_INPUT_HSHIFTER_BUTTON_ONE = 0x009,
  G27_INPUT_HSHIFTER_BUTTON_TWO = 0x010,
  G27_INPUT_HSHIFTER_BUTTON_THREE = 0x011,
  G27_INPUT_HSHIFTER_BUTTON_FOUR = 0x012,
  G27_INPUT_HSHIFTER_BUTTON_FIVE = 0x013,
  G27_INPUT_HSHIFTER_BUTTON_SIX = 0x014,
  G27_INPUT_HSHIFTER_BUTTON_SEVEN = 0x015,
  G27_INPUT_HSHIFTER_BUTTON_EIGHT = 0x016,
  G27_INPUT_HSHIFTER_DPAD_UP = 0x017,
  G27_INPUT_HSHIFTER_DPAD_DOWN = 0x018,
  G27_INPUT_HSHIFTER_DPAD_LEFT = 0x019,
  G27_INPUT_HSHIFTER_DPAD_RIGHT = 0x020,
  G27_INPUT_HSHIFTER_FIRST_GEAR = 0x021,
  G27_INPUT_HSHIFTER_SECOND_GEAR = 0x022,
  G27_INPUT_HSHIFTER_THIRD_GEAR = 0x023,
  G27_INPUT_HSHIFTER_FOURTH_GEAR = 0x024,
  G27_INPUT_HSHIFTER_FIFTH_GEAR = 0x025,
  G27_INPUT_HSHIFTER_SIXTH_GEAR = 0x026,
  G27_INPUT_HSHIFTER_REVERSE_GEAR = 0x027,
};

struct G27_INPUT_PEDAL {
  unsigned short throttle;
  unsigned short brake;
  unsigned short clutch;
};

}  // namespace g27
}  // namespace hid
}  // namespace ecu

#endif  // HID_G27_G27_HID_H_
