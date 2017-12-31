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
#include <iostream>

#include "gflags/gflags.h"
#include "g3log/g3log.hpp"
#include "g3log/logworker.hpp"
#include <QApplication>
#include <QDir>
#include <QDebug>

#include <ECU/ECU-version.h>
#include "ui/mainwindow.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);

  auto command_line = GetCommandLineW();
  int argc;
  wchar_t** argv = CommandLineToArgvW(command_line, &argc);
  if (!argv) {
    return 1;
  }

  gflags::SetVersionString("0.0");

  int argca = argc;
  auto argva = reinterpret_cast<char**>(alloca(sizeof(char*) * argca));
  for (int n = 0; n < argca; n++) {
    size_t len = wcslen(argv[n]);
    argva[n] = reinterpret_cast<char*>(alloca(len + 1));
    std::wcstombs(argva[n], argv[n], len + 1);
  }
  gflags::ParseCommandLineFlags(&argc, &argva, true);

  QApplication ecu(argc, argva);
  Mainwindow window;
  window.setWindowTitle(ECU_NAME_VERSION);
  window.show();

  gflags::ShutDownCommandLineFlags();
  LocalFree(argv);
  return ecu.exec();
}
