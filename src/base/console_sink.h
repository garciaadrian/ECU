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

#ifndef BASE_CONSOLE_SINK_H_
#define BASE_CONSOLE_SINK_H_

#include <string>
#include <iostream>
#include <Windows.h>

#include "g3log/logmessage.hpp"

namespace ecu {
namespace log {

void clear(HANDLE console);

class ConsoleSink {
 public:
  ConsoleSink(std::wstring title);
  ~ConsoleSink();

  void ReceiveLogMessage(g3::LogMessageMover logEntry);

 private:
  std::wstring title_;
  HANDLE std_out_;
};

}  // namespace log
}  // namespace ecu

#endif
