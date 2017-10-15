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

#include "base/file_watcher.h"

namespace ecu {

bool FileExists(const std::wstring& filename) {
  DWORD dw_attrib = GetFileAttributes(filename.c_str());

  return (dw_attrib != INVALID_FILE_ATTRIBUTES &&
          !(dw_attrib & FILE_ATTRIBUTE_DIRECTORY));
}
  
bool HasWritten(FileTime in) {
  // Invalid filetime
  if (in.creation_time_.dwLowDateTime == 0)
    return false;
  
  ULARGE_INTEGER old_time;
  old_time.LowPart = in.lastwrite_time_.dwLowDateTime;
  old_time.HighPart = in.lastwrite_time_.dwHighDateTime;
  
  GetFileTime(in.file_, &in.creation_time_, &in.lastaccess_time_, &in.lastwrite_time_);

  ULARGE_INTEGER new_time;
  new_time.LowPart = in.lastwrite_time_.dwLowDateTime;
  new_time.HighPart = in.lastwrite_time_.dwHighDateTime;

  if (new_time.QuadPart > old_time.QuadPart)
    return true;
  else
    return false;
}

bool CreateFileWatch(const std::wstring& filename, FileTime in) {
  if (!FileExists(filename)) {
    return false;
  }

  HANDLE file = CreateFile(filename.c_str(), GENERIC_READ,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                           nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    return false;
  }

  GetFileTime(file, &in.creation_time_, &in.lastaccess_time_, &in.lastwrite_time_);
  in.file_ = file;
  in.filename_ = filename;
  return true;
}
  
}  // namespace ecu
