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

#include "hid/input_system.h"

#include <fstream>

#include "g3log/g3log.hpp"

#include "hid/console_commands.h"
#include "hid/g27/g27_hid.h"

namespace ecu {
namespace hid {

std::vector<std::unique_ptr<hid::InputDriver>> CreateInputDrivers(
    ui::Window* window) {
  std::vector<std::unique_ptr<hid::InputDriver>> drivers;

  drivers.emplace_back(ecu::hid::g27::Create(window));
  return drivers;
}

InputSystem::InputSystem(ecu::ConsoleSystem* console_system)
    : console_system_(console_system) {}

InputSystem::~InputSystem() {

}

void InputSystem::AddDriver(std::unique_ptr<hid::InputDriver> driver) {
  drivers_.push_back(std::move(driver));
}

void InputSystem::AddCommand(std::unique_ptr<hid::InputCommand> command) {
  commands_.push_back(std::move(command));
}

void InputSystem::Bind(uint16_t key, const std::string& command_name) {
  for (auto& input_command : commands_) {
    if (input_command->name() == command_name) {
      bind_map_.emplace(key, input_command.get());
    }
  }
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
    if (packet_id == driver->GetDeviceId()) {
      driver->GetState(e);
    }
  }

  return 1;
};

std::vector<std::unique_ptr<ConCommand>> InputSystem::CreateConsoleCommands() {
  std::vector<std::unique_ptr<ConCommand>> commands;
  commands.push_back(std::make_unique<BindCommand>(this));

  return commands;
}

void MoveInputCommands(std::vector<std::unique_ptr<ecu::hid::InputCommand>> to,
                       std::vector<std::unique_ptr<ecu::hid::InputCommand>> from) {
  for (auto& command : from) {
    to.push_back(std::move(command));
  }
}

void MoveInputCommands(ecu::hid::InputSystem* input_system,
                       std::vector<std::unique_ptr<ecu::hid::InputCommand>>& commands) {
  for (auto& command : commands) {
    input_system->AddCommand(std::move(command));
  }
}


}  // namespace hid
}  // namespace ecu
