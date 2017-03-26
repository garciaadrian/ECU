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
#include <server/websocket.h>
#include <base/debug.h>
#include <db/sqlite.h>
#include <hid/hid.h>
#include <Hidsdi.h>
#include <process.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <json.hpp>

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

struct WebsocketSink {
  void ReceiveLogMessage(g3::LogMessageMover logEntry) {
    WORKER *list = get_active_sockets();

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
      if (list[i].socket != INVALID_SOCKET && list[i].in_use) {

        wchar_t buffer[65536] = {0};
        int num_chars = 0;

        using json = nlohmann::json;
        
        json j2 = {
          {"file_path", logEntry._move_only._file_path},
          {"file", logEntry._move_only._file},
          {"line", logEntry._move_only._line},
          {"function", logEntry._move_only._function},
          {"level", logEntry._move_only.level()},
          {"message", logEntry._move_only._message},
          {"expression", logEntry._move_only._expression},
        };

        std::string msg = j2.dump();

        /* message_wide does not preserve underlying UTF-8 encoding! */
        std::wstring msg_wide = std::wstring(msg.begin(), msg.end());
        
        /* num_chars = swprintf(buffer, sizeof(buffer), */
        /*                      L"{ \"message\": \"%s\"}", msg_wide.c_str()); */
        
        num_chars = swprintf(buffer, sizeof(buffer),
                             L"%s", msg_wide.c_str());
        
        send_frame(&list[i], buffer, num_chars);  /* num_chars is not size of the message!! */
        message_queue.push_back(logEntry);
      }
    }

    free(list);
  }

  std::vector<g3::LogMessageMover> message_queue;
};

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
  
  if (1) { // !config->headless
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
  auto sinkHandle = worker->addSink(std2::make_unique<WebsocketSink>(),
                                    &WebsocketSink::ReceiveLogMessage);
  auto handle = worker->addDefaultLogger("log", "./");
  g3::initializeLogging(worker.get());

  configuration *config = ecu_init();
  
  while (!config->configured) {
    LOGF(INFO, "Waiting for iRacing...");
    setup_weekend(config);
  }

  struct MHD_Daemon* daemon;
  daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                            &answer_to_connection, NULL, MHD_OPTION_END);
  
  if (daemon == NULL)
    LOGF(FATAL, "Failed to start HTTP server");
 
  if (!RegisterClassEx(&window))
    LOGF(FATAL, "Failed to register window");

  HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, szWindowClass, szTitle,
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             500, 100, HWND_MESSAGE, NULL, hInstance, NULL);
  
  if (!hWnd)
    LOGF(FATAL, "Failed to create window handle");

  g_Running = true;

  register_devices(hWnd);
  enumerate_devices();

  HANDLE wait_handles[2];
  wait_handles[0] = config->dw_change_handle;
  
  unsigned int tid = 0;

  HANDLE exit_event = CreateEvent(NULL, TRUE, FALSE, NULL);
  wait_handles[1] = (HANDLE)_beginthreadex(NULL, 0, ws_start_daemon,
                                           (void*)exit_event, 0, &tid);
  SetThreadName("Websocket Thread Manager", tid);
  
  while (g_Running) {
    MSG msg;
    int rc;
    int wait_count = 2; /* dw_change_handle + Websocket Thread Manager*/


    rc = MsgWaitForMultipleObjects(wait_count, wait_handles,
                                   FALSE, INFINITE, QS_ALLINPUT);
    
    if (rc == WAIT_OBJECT_0 + wait_count) {
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_CLOSE) {
          LOGF(DEBUG, "CEF Requested closing. signaling exit event");
          SetEvent(exit_event);
          /* Wait for 15 secs incase a thread hangs? */
          WaitForMultipleObjects(wait_count-1, wait_handles+1, true, INFINITE);
          LOGF(DEBUG, "All threads have closed. Closing ECU!");
          free(config);
          g_Running = false;
          CefShutdown();
          /* CefQuitMessageLoop(); */
          return 0;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    
    else if (rc >= WAIT_OBJECT_0
             && rc < WAIT_OBJECT_0 + wait_count) {
      int index = rc - WAIT_OBJECT_0;
      
      if (config->ibt_sorting) {
        sort_ibt_directory(config->telemetry_path);
      }
    }

    else if (rc == WAIT_TIMEOUT) {

    }

    else if (rc >= WAIT_ABANDONED_0
             && rc < WAIT_ABANDONED_0 + wait_count) {
      int index = rc - WAIT_ABANDONED_0;
      /* A thread died that owned a mutex */
    }

    else {
      // something went wrong
    }
  }
  return 0;
}
