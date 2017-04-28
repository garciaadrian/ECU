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
#include "graphics/app.h"
#include "include/cef_sandbox_win.h"

DEFINE_bool(cef, false, "Toggles GUI");

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  LRESULT Result = 0;

  switch (Msg) {
    case WM_INPUT: {
    }

    default: { Result = DefWindowProc(hWnd, Msg, wParam, lParam); }
  }

  return Result;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  static TCHAR szWindowClass[] = _T("iracingECU");
  static TCHAR szTitle[] = _T("iRacing ECU");

  WNDCLASSEX window;
  window.cbSize = sizeof(WNDCLASSEX);
  window.style = CS_HREDRAW | CS_VREDRAW;
  window.lpfnWndProc = WndProc;
  window.cbClsExtra = 0;
  window.cbWndExtra = 0;
  window.hInstance = hInstance;
  window.hIcon = 0;
  window.hCursor = LoadCursor(NULL, IDC_ARROW);
  window.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  window.lpszMenuName = NULL;
  window.lpszClassName = szWindowClass;
  window.hIconSm = 0;

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

  if (FLAGS_cef) {
    CefEnableHighDPISupport();

    void* sandbox_info = NULL;
#ifdef CEF_USE_SANDBOX
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    CefMainArgs main_args(hInstance);

    int exit_code = CefExecuteProcess(main_args, NULL, sandbox_info);

    if (exit_code >= 0)
      LOGF(FATAL, "CEF returned with exit code %d", exit_code);

    CefSettings settings;
    settings.remote_debugging_port = 9991;
    settings.log_severity = LOGSEVERITY_WARNING;
    settings.multi_threaded_message_loop = true;

#ifndef CEF_USE_SANDBOX
    settings.no_sandbox = true;
#endif
    CefRefPtr<SimpleApp> app(new SimpleApp(GetCurrentThreadId()));

    CefInitialize(main_args, settings, app.get(), sandbox_info);
  }

  auto worker = g3::LogWorker::createLogWorker();
  auto handle = worker->addDefaultLogger("log", "./");
  g3::initializeLogging(worker.get());

  if (!RegisterClassEx(&window)) LOGF(FATAL, "Failed to register main window");

  HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle,
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             500, 100, HWND_MESSAGE, NULL, hInstance, NULL);

  if (!hWnd) LOGF(FATAL, "Failed to create window handle");

  return 0;
}
