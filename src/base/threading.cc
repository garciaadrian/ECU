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

#include "base/threading.h"

#include <windows.h>

namespace ecu {
namespace threading {

struct THREADNAME_INFO {
  DWORD dwType;
  LPCSTR szName;
  DWORD dwThreadID;
  DWORD dwFlags;
};

void set_name(DWORD thread_id, const std::string& name) {
  if (!IsDebuggerPresent()) {
    return;
  }

  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = name.c_str();
  info.dwThreadID = thread_id;
  info.dwFlags = 0;

  __try {
    RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR),
                   reinterpret_cast<ULONG_PTR*>(&info));
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
}

void set_name(const std::string& name) {
  set_name(static_cast<DWORD>(-1), name);
}

}  // namespace threading
}  // namespace ecu
