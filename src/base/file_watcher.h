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

#ifndef BASE_FILE_WATCHER_H_
#define BASE_FILE_WATCHER_H_

#include <string>

#include <Windows.h>

namespace ecu {

struct FileTime {
  std::wstring filename_;
  HANDLE file_{};
  FILETIME creation_time_ = {0};
  FILETIME lastaccess_time_ = {0};
  FILETIME lastwrite_time_ = {0};

  bool is_valid() {
    return creation_time_.dwLowDateTime != 0;
  }
};

bool FileExists(const std::wstring& filename);
bool HasWritten(FileTime in);
bool CreateFileWatch(const std::wstring& filename, FileTime in);

}  // namespce ecu

#endif // BASE_FILE_WATCHER_H_
