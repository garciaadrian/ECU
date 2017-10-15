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

  ::iracing::Iracing conn;
  vjoy::Feeder device("joy.json");

  device.SetButtonInput(1, iracing::BRAKE_BIAS_INC);
  device.SetButtonInput(2, iracing::BRAKE_BIAS_DEC);

  ecu::websocket::WebsocketServer server;

  server.Run();

  ecu::vm::LuaVM jit;
  jit.LoadFile("data/default.lua");

  FileTime default_lua;
  bool reload = CreateFileWatch(L"data/default.lua", default_lua);
  if (reload == false) {
    LOGF(g3::WARNING, "Unable to live reload default.lua");
  }

  auto tick = conn.GetTick();

  auto session_string = conn.GetSessionInfo();
  
  while (!should_exit_) {
    if (reload && HasWritten(default_lua)) {
      jit.LoadFile("data/default.lua");
    }
    tick = conn.GetTick();
    if (!tick.IsValid()) {
      continue;
    }
    brakes_.GetState(tick, device, session_string);
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
