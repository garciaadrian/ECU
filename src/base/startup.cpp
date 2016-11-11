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

#include "startup.h"
#include <base/loop.h>
#include <Strsafe.h>

configuration *ecu_init()
{
  configuration *config = (configuration *)malloc(sizeof(configuration));
  config->configured = false;
  
  static char debug[128];
  PWSTR telemetry_path;

  if (GetModuleFileName(NULL, config->path, MAX_PATH_UNICODE) == 0) {
    DEXIT_PROCESS(L"Failed to get Module FileName");
  }

  SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &telemetry_path);
  StringCchCopy(config->telemetry_path, MAX_PATH_UNICODE, telemetry_path);
  StringCchCat(config->telemetry_path, MAX_PATH_UNICODE, L"\\iRacing\\telemetry\\");

  sort_ibt_directory(config->telemetry_path);
  
  config->dw_change_handle = FindFirstChangeNotification(config->telemetry_path, FALSE,
                                                         FILE_NOTIFY_CHANGE_FILE_NAME);
  
  size_t len = wcsnlen_s(config->path, MAX_PATH_UNICODE);
  
  /* TODO: add support for changing executable name */
  config->path[len - 7] = '\0'; /* ECU.exe */
  
  // Don't execute anything until iRacing is running
  DEBUG_OUTA("[STARTUP]: Init ECU\n", debug);
  init_db(config);
  return config;
}
