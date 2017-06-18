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

#include "hid/input_system.h"
#include "hid/g27/g27_hid.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window) {
  std::vector<std::unique_ptr<hid::InputDriver>> drivers;

  drivers.emplace_back(ecu::hid::g27::Create(window));
  return drivers;
}

InputSystem::InputSystem(ecu::ui::Window* window) : window_(window) {}

InputSystem::~InputSystem() {}

void InputSystem::AddDriver(std::unique_ptr<hid::InputDriver> driver) {
  drivers_.push_back(std::move(driver));
}

void InputSystem::AddCommand(std::unique_ptr<InputCommand> command) {
  commands_.push_back(std::move(command));
}

std::pair<int, int> InputSystem::GetPacketId(HANDLE device) {
  RID_DEVICE_INFO device_info = {0};
  UINT size = sizeof(device_info);

  device_info.cbSize = sizeof(RID_DEVICE_INFO);
  GetRawInputDeviceInfo(device, RIDI_DEVICEINFO, &device_info, &size);
  if (device_info.dwType == RIM_TYPEHID) {
    return std::make_pair(device_info.hid.dwVendorId,
                          device_info.hid.dwProductId);
  } else {
    return std::make_pair(0, 0);
  }
}

int InputSystem::GetState(ecu::ui::RawInputEvent* e) {
  unsigned int buffer_size = 0;
  GetRawInputData((HRAWINPUT)e->lparam(), RID_INPUT, nullptr, &buffer_size,
                  sizeof(RAWINPUTHEADER));
  RAWINPUT* buffer = new RAWINPUT[buffer_size];

  GetRawInputData((HRAWINPUT)e->lparam(), RID_INPUT, buffer, &buffer_size,
                  sizeof(RAWINPUTHEADER));
  auto packet_id = GetPacketId(buffer->header.hDevice);
  delete[] buffer;

  // device is a keyboard or mouse
  if (packet_id.first == 0 && packet_id.second == 0) {
    return 0;
  }

  for (auto& driver : drivers_) {
    if (packet_id == driver->GetIdPair()) {
      driver->GetState(e);
    }
  }

  return 1;
};

}  // namespace hid
}  // namespace ecu
