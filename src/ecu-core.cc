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

#include "version.h"
#include "gflags/gflags.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include "json.hpp"

#include "ui/window.h"
#include "ui/loop.h"
#include "ui/menu_item.h"
#include "base/console_sink.h"
#include "base/threading.h"
#include "hid/input_system.h"
#include "hid/input_driver.h"
#include "hid/g27/g27_hid.h"

// TODO(garciaadrian): Really messy, move this somwhere else
using ecu::ui::MenuItem;
  
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  
  auto worker = g3::LogWorker::createLogWorker();
  auto handle = worker->addDefaultLogger("log", "./");

  auto console_sink = worker->addSink(
      std2::make_unique<ecu::log::ConsoleSink>(std::wstring(L"ECU Log")),
      &ecu::log::ConsoleSink::ReceiveLogMessage);

  g3::initializeLogging(worker.get());

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

  std::stringstream title_stream;
  title_stream <<  "Build: " << ECU_BUILD_COMMIT_SHORT << " " << "Branch: "
               << ECU_BUILD_BRANCH << " " << ECU_BUILD_DATE;
  std::string title = title_stream.str();
  std::wstring title_wide(title.begin(), title.end());
  
  auto loop = std::make_unique<ecu::ui::Loop>();
  
  auto window = std::make_unique<ecu::ui::Window>(title_wide);

  loop->PostSynchronous([&window]() {
      ecu::threading::set_name("Win32 Loop");
      if (!window->Initialize()) {
        LOGF(DEBUG, "Failed to create debug window");
        return;
      }
      LOGF(DEBUG, "Created window");
    });

  auto main_menu = MenuItem::Create(MenuItem::Type::kNormal);
  auto file_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&File");
  {
    file_menu->AddChild(ecu::ui::MenuItem::Create(MenuItem::Type::kString,
                                                  L"&Close",
                                                  L"Alt+F4",
                                                  [&window]() { window->Close(); }));
  }
  main_menu->AddChild(std::move(file_menu));
  auto debug_menu = MenuItem::Create(MenuItem::Type::kPopup, L"&Debug");
  {
    debug_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                          L"Toggle Profiler &Display", L"F3",
                                          []() {  }));
    debug_menu->AddChild(MenuItem::Create(MenuItem::Type::kString,
                                          L"&Pause/Resume Profiler", L"`",
                                          []() { }));
  }
  main_menu->AddChild(std::move(debug_menu));
  window->set_main_menu(std::move(main_menu));

  
  window->Resize(1280, 720);

  window->on_closed.AddListener(
      [&loop, &window](ecu::ui::UIEvent* e) {
        loop->Quit();        
      });
  window->on_quit.AddListener([&window](ecu::ui::UIEvent* e) { window.reset(); });

  auto drivers = ecu::hid::CreateInputDrivers(window.get());

  loop->AwaitQuit();

  window.reset();
  loop.reset();

  google::ShutDownCommandLineFlags();
  LocalFree(argv);
  return 0;
}
