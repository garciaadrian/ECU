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
#include <resource.h>

void load_file_resource(int name, int type, DWORD& size, const char*& data)
{
  // TODO: Add error checking
  HMODULE handle = GetModuleHandle(NULL);
  HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name),
                          MAKEINTRESOURCE(type));
  HGLOBAL rcData = LoadResource(handle, rc);
  size = SizeofResource(handle, rc);
  data = static_cast<const char*>(LockResource(rcData));
}

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

  if (MATCH("deployment", "angle")) {
    config->angle = atoi(value);
  }

  if (MATCH("deployment", "derateFixed")) {
    config->derate_fixed = atoi(value);
  }

  if (MATCH("deployment", "normRateFixed")) {
    config->normrate_fixed = atoi(value);
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

  if (GetModuleFileName(NULL, config->path, MAX_PATH_UNICODE) == 0)
    LOGF(FATAL, "Failed to get Module Filename. GLE: %d", GetLastError());
  
  SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &telemetry_path);
  StringCchCopy(config->telemetry_path, MAX_PATH_UNICODE, telemetry_path);
  StringCchCat(config->telemetry_path, MAX_PATH_UNICODE, L"\\iRacing\\telemetry\\");
  
  config->dw_change_handle = FindFirstChangeNotification(config->telemetry_path, FALSE,
                                                         FILE_NOTIFY_CHANGE_FILE_NAME);
  
  size_t len = wcsnlen_s(config->path, MAX_PATH_UNICODE);
  
  config->path[len - 7] = '\0'; // ECU.exe
  
  wchar_t temp_path[MAX_PATH_UNICODE];
  wcscpy_s(temp_path, MAX_PATH_UNICODE, config->path);
  
  swprintf_s(config->path, MAX_PATH_UNICODE, L"\\\\?\\%s", temp_path);
  
  HANDLE config_ini_handle;
  DWORD config_ini_size;

  if (!PathFileExists(L"config.ini")) {
    config_ini_handle = CreateFile(L"config.ini", GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                   CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (config_ini_handle == INVALID_HANDLE_VALUE)
      LOGF(FATAL, "Invalid Handle. GLE: %d", GetLastError());

    DWORD size = 0;
    const char *data = NULL;
    load_file_resource(CONFIG_TEXTFILE, TEXTFILE, size, data);
    
    // The text in the file resource might not be null terminated
    char *buffer = (char*)malloc(size+1);
    memcpy(buffer, data, size);
    buffer[size] = 0;

    DWORD bytes_written;
    
    if (!WriteFile(config_ini_handle, buffer, size, &bytes_written, NULL)) {
      CloseHandle(config_ini_handle);
      LOGF(FATAL, "Can't write to config.ini. GLE: %d", GetLastError());
    }
    CloseHandle(config_ini_handle);
    free(buffer);
  }
  
  if (ini_parse("config.ini", config_ini_handler, config) < 0)
    LOGF(FATAL, "Can't load config.ini. GLE: %d", GetLastError());

  if (config->ibt_sorting) {
    sort_ibt_directory(config->telemetry_path);
  }

  
  /* Don't execute anything until iRacing is running */
  LOGF(DEBUG, "Init ECU");
  init_db(config);
  return config;
}
