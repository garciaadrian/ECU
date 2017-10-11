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

#include <Windows.h>
#include <TimeAPI.h>
#include <tchar.h>
#include <process.h>
#include <Shellapi.h>
#include <fstream>

#include "gflags/gflags.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"

#include "ui/window.h"
#include "ui/loop.h"
#include "ui/menu_item.h"
#include "base/console_sink.h"
#include "base/threading.h"
#include "base/cvars.h"
#include "base/console_system.h"
#include "hid/input_command.h"
#include "hid/input_system.h"
#include "hid/input_driver.h"
#include "hid/g27/g27_hid.h"
#include "libir/iracing.h"
#include "libir/events.h"
#include "car/ecu.h"
#include "scripting/vm.h"

using ecu::ui::MenuItem;

DEFINE_bool(log, true, "Create a new log file on startup");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  auto command_line = GetCommandLineW();
  int argc;
  wchar_t** argv = CommandLineToArgvW(command_line, &argc);
  if (!argv) {
    return 1;
  }

  // google::SetVersionString("0.1");

  int argca = argc;
  char** argva = reinterpret_cast<char**>(alloca(sizeof(char*) * argca));
  for (int n = 0; n < argca; n++) {
    size_t len = wcslen(argv[n]);
    argva[n] = reinterpret_cast<char*>(alloca(len + 1));
    std::wcstombs(argva[n], argv[n], len + 1);
  }
  // google::ParseCommandLineFlags(&argc, &argva, true);

  auto console_system = std::make_unique<ecu::ConsoleSystem>();
  
  auto worker = g3::LogWorker::createLogWorker(); 
  if (FLAGS_log)
    auto handle = worker->addDefaultLogger("log", "./");    

  auto console_sink = worker->addSink(
      std2::make_unique<ecu::log::ConsoleSink>(std::wstring(L"ECU Log")),
      &ecu::log::ConsoleSink::ReceiveLogMessage);

  g3::initializeLogging(worker.get());

  // std::stringstream title_stream;
  // title_stream <<  "Build: " << ECU_BUILD_COMMIT_SHORT << " " << ECU_BUILD_DATE;
  // std::string title = title_stream.str();
  // std::wstring title_wide(title.begin(), title.end());
  
  auto loop = std::make_unique<ecu::ui::Loop>();
  
  auto window = std::make_unique<ecu::ui::Window>(L"ECU");

  loop->PostSynchronous([&window]() {
      ecu::threading::set_name("Win32 Loop");
      if (!window->Initialize()) {
        LOGF(g3::DEBUG, "Failed to create debug window\n");
        return;
      }
      LOGF(g3::DEBUG, "Created window\n");
    });

  auto main_menu = MenuItem::Create(MenuItem::Type::kNormal);
  auto file_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&File");
  {
    file_menu->AddChild(ecu::ui::MenuItem::Create(
        MenuItem::Type::kString,
        L"&Close",
        L"Alt+F4",
        [&window]() { window->Close(); }));
  }
  main_menu->AddChild(std::move(file_menu));
  auto debug_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&Help");
  {
    debug_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                          L"Toggle Profiler &Display", L"F3",
                                          []() {} ));
    debug_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                          L"&Feedback/Questions", L"",
                                          []() {} ));
  }
  main_menu->AddChild(std::move(debug_menu));
  window->set_main_menu(std::move(main_menu));
  
  window->Resize(1280, 720);

  // Move this to CreateInputSystem()
  std::unique_ptr<ecu::hid::InputSystem> input_system_ =
      std::make_unique<ecu::hid::InputSystem>(console_system.get());

  auto drivers = ecu::hid::CreateInputDrivers(window.get());
  for (size_t i = 0; i < drivers.size(); ++i) {
    input_system_->AddDriver(std::move(drivers[i]));
  }
  
  ecu::hid::MoveInputCommands(input_system_.get(), ecu::hid::CreateInputCommands());
  ecu::hid::MoveInputCommands(input_system_.get(), ecu::car::CreateInputCommands());
  
  auto control_unit = std::make_unique<ecu::car::ControlUnit>(console_system.get());
  
  ecu::MoveConsoleCommands(console_system->command_system(),
                           input_system_->CreateConsoleCommands());
  ecu::MoveConsoleCommands(console_system->command_system(),
                           control_unit->CreateConsoleCommands());
  
  console_system->ParseAutoExec("autoexec.cfg");

  window->on_closed.AddListener(
      [&loop, &window, &input_system_, &control_unit](ecu::ui::UIEvent* e) {
        control_unit->Quit();
        loop->Quit();
      });

  window->on_quit.AddListener([&window](ecu::ui::UIEvent* e) { window.reset(); });
  
  window->on_raw_input.AddListener([&input_system_](ecu::ui::RawInputEvent* e) {
      input_system_->GetState(e);
    });

  LuaVM jit;
  jit.LoadFile("default.lua");

  control_unit->AwaitQuit();
  loop->AwaitQuit();
  
  window.reset();
  loop.reset();

  // google::ShutDownCommandLineFlags();
  LocalFree(argv);
  return 0;
}
