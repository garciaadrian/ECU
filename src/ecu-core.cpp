/**
*******************************************************************************
*                                                                             *
* ECU: iRacing MP4-30 Performance Analysis Project                            *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*                                                                             *
* Author: Adrian Garcia Cruz <garcia.adrian.cruz@gmail.com>                   *
*******************************************************************************
*/

#include <Windows.h>
#include <tchar.h>
#include <TimeAPI.h>

#include <base/startup.h>
#include <base/loop.h>
#include <base/websocket.h>
#include <db/sqlite.h>
#include <hid/hid.h>
#include <Hidsdi.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#include "graphics/app.h"
#include "include/cef_sandbox_win.h"

#include "server/http.h"

#define _WIN32_WINNT 0x0602
#define STANDALONE 0
#define WM_SOCKET 104

bool g_Running = false;

#ifdef _DEBUG
bool debug = true;
#else
bool debug = false;
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  
  LRESULT Result = 0;

  switch (Msg) {
      
    case WM_LBUTTONDOWN: {
      MessageBox(hWnd, L"You clicked me~", L"Click", MB_OK | MB_ICONINFORMATION);
      break;
    }

    case WM_CLOSE: {
      OutputDebugString(_T("WM_CLOSE\n"));
      g_Running = false;
      break;
    }

    case WM_INPUT: {
      hid_poll(lParam);
      break;
    }

    case WM_DESTROY: {
      OutputDebugString(_T("WM_DESTROY"));
      g_Running = false;
      break;
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
   
  if (!debug) {
    CefEnableHighDPISupport();

    void* sandbox_info = NULL;
#ifdef CEF_USE_SANDBOX
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    CefMainArgs main_args(hInstance);

    int exit_code = CefExecuteProcess(main_args, NULL, sandbox_info);

    if (exit_code >= 0) {
      return exit_code;
    }

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

  ws_daemon *ws = (ws_daemon*)malloc(sizeof(ws_daemon));
  ws_start_daemon(ws);

  struct MHD_Daemon* daemon;
  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                            &answer_to_connection, NULL, MHD_OPTION_END);
  
  if (NULL == daemon) {
    DEXIT_PROCESS(L"Failed to start http server", GetLastError());
  }
 
  if (!RegisterClassEx(&window)) {
    return __LINE__;
  }

  HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle,
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             500, 100, HWND_MESSAGE, NULL, hInstance, NULL);

  if (!hWnd) {
    DEXIT_PROCESS(L"Window handle failed to create", GetLastError());
  }

  /* ShowWindow(hWnd, nCmdShow); */
  /* UpdateWindow(hWnd); */

  configuration *config = ecu_init();
  
  while (!config->configured) {
    LOGF(INFO, "Waiting for iRacing...");
    setup_weekend(config);
  }

  g_Running = true;

  register_devices(hWnd);
  enumerate_devices();
  
  while (g_Running) {
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0)) {
      if (msg.message == WM_CLOSE) {
        free(config);
        free(ws);
        g_Running = false;
        CefShutdown();
        return 0;
      }
      
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   
    }
    
  }
  return 0;
}
