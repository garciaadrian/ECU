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
#include <Dbghelp.h>
#include <ini.h>

int config_ini_handler(void *user, const char *section, const char *name,
                       const char *value)
{
  configuration *config = (configuration *)user;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

  if (MATCH("core", "ibt_sorting")) {
    config->ibt_sorting = atoi(value);
  }

  if (MATCH("core", "headless")) {
    config->headless = atoi(value);
  }
  
  if (MATCH("network", "ip")) {
    config->ip = value;
  }
  
  else {
    return 0;
  }
  return 0;
}

configuration *ecu_init()
{
  
  configuration *config = (configuration *)malloc(sizeof(configuration));
  
  config->ibt_sorting = false;
  config->configured = false;
  
  PWSTR telemetry_path = NULL;

  if (GetModuleFileName(NULL, config->path, MAX_PATH_UNICODE) == 0) {
    DEXIT_PROCESS(L"Failed to get Module Filename", GetLastError());
  }
  
  SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &telemetry_path);
  StringCchCopy(config->telemetry_path, MAX_PATH_UNICODE, telemetry_path);
  StringCchCat(config->telemetry_path, MAX_PATH_UNICODE, L"\\iRacing\\telemetry\\");
  
  config->dw_change_handle = FindFirstChangeNotification(config->telemetry_path, FALSE,
                                                         FILE_NOTIFY_CHANGE_FILE_NAME);
  
  size_t len = wcsnlen_s(config->path, MAX_PATH_UNICODE);
  
  /* TODO: add support for changing executable name */
  config->path[len - 7] = '\0'; /* ECU.exe */
  
  wchar_t temp_path[MAX_PATH_UNICODE];
  wcscpy_s(temp_path, MAX_PATH_UNICODE, config->path);
  
  swprintf_s(config->path, MAX_PATH_UNICODE, L"\\\\?\\%s", temp_path);
  
  HANDLE config_ini_handle;
  DWORD config_ini_size;

  if (!PathFileExists(L"config.ini")) {
    config_ini_handle = CreateFile(L"config.ini", GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                   CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (config_ini_handle == INVALID_HANDLE_VALUE) {
      DEXIT_PROCESS(L"Invalid Handle", GetLastError());
    } 

    const char ini_text[] = "[core]\r\n"
                            "ibt_sorting = 0\r\n"
                            "headless = 0\r\n"
                            "[network]\r\n"
                            "ip = 192.168.29.29\r\n";
    DWORD bytes_written;
    
    if (!WriteFile(config_ini_handle, ini_text, sizeof(ini_text)/sizeof(ini_text[0]),
                   &bytes_written, NULL)) {
      DEXIT_PROCESS(L"Can't write to config.ini", GetLastError());
    }
    CloseHandle(config_ini_handle);
  }
  
  if (ini_parse("config.ini", config_ini_handler, config) < 0) {
    DEXIT_PROCESS(L"Can't load config.ini", 0);
  }

  if (config->ibt_sorting) {
    sort_ibt_directory(config->telemetry_path);
  }

  
  /* Don't execute anything until iRacing is running */
  LOGF(DEBUG, "Init ECU");
  init_db(config);
  return config;
}
