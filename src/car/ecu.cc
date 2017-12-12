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

#include "car/ecu.h"

#include <Windows.h>
#include <chrono>

#include "g3log/g3log.hpp"

#include "base/threading.h"
#include "base/console_command.h"
#include "base/file_watcher.h"
#include "server/websocket.h"
#include "vjoy-feeder/feeder.h"
#include "libir/iracing.h"
#include "scripting/vm.h"

namespace ecu {
namespace car {

ControlUnit::ControlUnit(ConsoleSystem* console_system)
    : console_(console_system) {
  device_.LoadConfiguration("joy.json");
  thread_ = std::thread(&ControlUnit::ThreadMain, this);
}

ControlUnit::~ControlUnit() {
  thread_.join();
}

void ControlUnit::Quit() {
  should_exit_ = true;
}

void ControlUnit::AwaitQuit() {
  quit_fence_.Wait();
}

void ControlUnit::ThreadMain() {
  LOGF(g3::DEBUG, "Starting Control Unit thread %d\n", GetCurrentThreadId());
  ecu::threading::set_name("Control Unit Thread");

  vjoy::SetDefaultDeviceButtons(device_);
  
  auto tick = iracing_conn_.GetTick();
  auto session_string = iracing_conn_.GetSessionInfo();

  ecu::vm::LuaVM jit;
  ecu::websocket::WebsocketServer server;
  
  while (!should_exit_) {
    tick = iracing_conn_.GetTick();
    if (!tick.IsValid()) {
      continue;
    }
    brakes_.GetState(tick, device_, session_string);
  }
  quit_fence_.Signal();
  LOGF(g3::DEBUG, "Stopping Control Unit thread\n");
}

std::vector<std::unique_ptr<ecu::ConCommand>> ControlUnit::CreateConsoleCommands() {
  std::vector<std::unique_ptr<ecu::ConCommand>> con_commands;
  auto brake_system_commands = brakes_.CreateConsoleCommands();

  for (auto& command : brake_system_commands) {
    con_commands.push_back(std::move(command));
  }

  return con_commands;
}

std::vector<std::unique_ptr<hid::InputCommand>> ControlUnit::CreateInputCommands() {
  std::vector<std::unique_ptr<hid::InputCommand>> input_commands;

  return input_commands;
}

std::vector<std::unique_ptr<ecu::CVar>> CreateCVars() {
  std::vector<std::unique_ptr<ecu::CVar>> cvars;

  return cvars;
}

std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands() {
  std::vector<std::unique_ptr<hid::InputCommand>> input_commands;

  return input_commands;
}

}  // namespace car
}  // namespace ecu
