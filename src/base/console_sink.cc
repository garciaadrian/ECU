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

#include "base/console_sink.h"

namespace ecu {
namespace log {

ConsoleSink::ConsoleSink(std::wstring title) {
  AllocConsole();
  SetConsoleTitle(title.c_str());
  std_out_ = GetStdHandle(STD_OUTPUT_HANDLE);
}

ConsoleSink::~ConsoleSink() { FreeConsole(); }

void ConsoleSink::ReceiveLogMessage(g3::LogMessageMover logEntry) {
  auto level = logEntry.get()._level;
  DWORD char_written = 0;
  std::stringstream message;
  message << logEntry.get().timestamp() << " " << logEntry.get().level() << ": "
          << logEntry.get().message();

  // TODO(garciaadrian): Unicode version not printing to console
  // GetLastError: 998
  WriteConsoleA(std_out_, message.str().c_str(), message.str().length(),
                &char_written, nullptr);
}

}  // namespace log
}  // namespace ecu
