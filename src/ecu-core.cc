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

#include <Windows.h>
#include <TimeAPI.h>
#include <tchar.h>
#include <process.h>
#include <Shellapi.h>

#include "gflags/gflags.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include "json.hpp"

#include "ui/window.h"
#include "base/console_sink.h"
#include "hid/input_system.h"
#include "hid/input_driver.h"
#include "hid/g27/g27_hid.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  auto worker = g3::LogWorker::createLogWorker();
  auto handle = worker->addDefaultLogger("log", "./");

  auto console_sink = worker->addSink(
      std2::make_unique<ecu::log::ConsoleSink>(std::wstring(L"ECU Log")),
      &ecu::log::ConsoleSink::ReceiveLogMessage);

  g3::initializeLogging(worker.get());

  ecu::ui::Window debug_window(std::wstring(L"ECU Debug"));
  LOGF(DEBUG, "Created window");

  if (!debug_window.Initialize()) {
    LOGF(DEBUG, "Failed to create debug window");
  }

  auto drivers = ecu::hid::CreateInputDrivers(&debug_window);
  int result = 0;

  auto command_line = GetCommandLineW();
  int argc;
  wchar_t** argv = CommandLineToArgvW(command_line, &argc);
  if (!argv) {
    return 1;
  }

  google::SetVersionString("1.0");

  int argca = argc;
  char** argva = reinterpret_cast<char**>(alloca(sizeof(char*) * argca));
  for (int n = 0; n < argca; n++) {
    size_t len = wcslen(argv[n]);
    argva[n] = reinterpret_cast<char*>(alloca(len + 1));
    std::wcstombs(argva[n], argv[n], len + 1);
  }

  google::ParseCommandLineFlags(&argc, &argva, true);
  google::ShutDownCommandLineFlags();
  LocalFree(argv);
  return 0;
}