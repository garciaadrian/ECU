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

#include "loop.h"

#include <irsdk/irsdk_client.h>
#include <irsdk/irsdk_defines.h>
#include <irsdk/yaml_parser.h>
#include "ir_helper.h"
#include <stdlib.h>
#include <Strsafe.h>
#include <io.h>
#include <fcntl.h>
#include <Shlobj.h>

void check_track(sqlite3 *db)
{
  static char buffer[512];
  static char sql[512];
  sqlite3_stmt *sql_stmt = {0};
  int rc = 0;

  char *ir_buffer;

  if (irsdkClient::instance().waitForData(16)) {
    ir_buffer = ir_get_variable("WeekendInfo:TrackDisplayName:", irsdk_getSessionInfoStr());
    sprintf_s(sql, "SELECT * FROM tracks WHERE name='%s'", ir_buffer);
    
    
    rc = sqlite3_prepare_v2(db, sql, sizeof(sql), &sql_stmt, NULL);
    handle_sql_rc(rc, sql, db, false, false, true);
    
    rc = sqlite3_step(sql_stmt);
    DEBUG_OUTA("[STARTUP]: Checking if current track is on the database\n", buffer);
        
    if (!handle_sql_rc(rc, sql, db, true, true, true)) {
      
      DEBUG_OUTA("[STARTUP]: Entry for track '%s' was not found in table `tracks`\n"
                 "[STARTUP]: Creating entry in table `tracks` for '%s'\n",
                 buffer, ir_buffer, ir_buffer);
      sprintf_s(sql, "INSERT INTO tracks(name) VALUES('%s')", ir_buffer);

      rc = sqlite3_prepare_v2(db, sql, sizeof(sql), &sql_stmt, NULL);
      handle_sql_rc(rc, sql, db, false, true, false);
      rc = sqlite3_step(sql_stmt);
      handle_sql_rc(rc, sql, db, true, true, false);
      
    } else {
      DEBUG_OUTA("[STARTUP]: Track %s found in database\n", buffer, ir_buffer);
      const unsigned char *sql_result = sqlite3_column_text(sql_stmt, 1);
    }

    free(ir_buffer);
  }
}

ibt *parse_ibt(intptr_t fd, wchar_t *file_name)
{
  
  FILE *file = 0;
  if (fd != -1)
    file = _fdopen(fd, "r");
  else {
    DEXIT_PROCESS(L"File descriptor invalid.", GetLastError());
  }

  if (file == 0) {
    size_t chars_converted;
    wchar_t message[255];
    swprintf_s(message, L"Invalid File: %s", file_name);
    DEXIT_PROCESS(message, GetLastError());
  }
  
  ibt *telemetry = (ibt *)malloc(sizeof(ibt));

  if (telemetry) {
    
    fread(&telemetry->header, 1, sizeof(irsdk_header), file);
    telemetry->session_string = (char *)malloc(telemetry->header.sessionInfoLen+1);
    fread(&telemetry->disk_subheader, 1, sizeof(irsdk_diskSubHeader), file);
  
    fseek(file, telemetry->header.sessionInfoOffset, SEEK_SET);
    
    fread(telemetry->session_string, 1, telemetry->header.sessionInfoLen, file);
  
    telemetry->session_string[telemetry->header.sessionInfoLen-1] = '\0';
  
    return telemetry;
  }
  
  int ret = 0;
  
  
}

wchar_t *num_to_month(int month)
{
  switch (month)
  {
    case 1:
      return L"January";
    case 2:
      return L"February";
    case 3:
      return L"March";
    case 4:
      return L"April";
    case 5:
      return L"May";
    case 6:
      return L"June";
    case 7:
      return L"July";
    case 8:
      return L"August";
    case 9:
      return L"September";
    case 10:
      return L"October";
    case 11:
      return L"November";
    case 12:
      return L"December";
    default:
      DEXIT_PROCESS(L"INVALID TIME MONTH", GetLastError());
  }
}

void sort_ibt_directory(wchar_t *directory)
{
  wchar_t debug[1024];
  
  WIN32_FIND_DATA files;
  HANDLE search;
  wchar_t directory_wildcard[MAX_PATH_UNICODE];
  wchar_t filename[MAX_PATH_UNICODE];
  wchar_t extension[4];
  
  StringCchCopy(directory_wildcard, MAX_PATH_UNICODE, directory);
  
  StringCchCat(directory_wildcard, MAX_PATH_UNICODE, L"*");

  if (!PathFileExists(directory))
    /* Path length limit is 248 characters inc. null terminator */
    SHCreateDirectory(NULL, directory);
  
  search = FindFirstFile(directory_wildcard, &files);

  if (search == INVALID_HANDLE_VALUE) {
    DEXIT_PROCESS(L"FindFirstFile returned invalid handle.", GetLastError());
  }

  do
  {
    int len = wcsnlen(files.cFileName, MAX_PATH);
    wcsncpy(extension, files.cFileName + (len - 3), 4);
    
    if (lstrcmp(extension, L"ibt") == 0) {
      StringCchCopy(filename, MAX_PATH_UNICODE, directory);
      StringCchCat(filename, MAX_PATH_UNICODE, files.cFileName);
      
      HANDLE file_handle;
      
      __try {
        file_handle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                                        OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
      }
      
      __except (EXCEPTION_EXECUTE_HANDLER) {
        if (INVALID_HANDLE_VALUE != file_handle)
          CloseHandle(file_handle);
        
        file_handle = INVALID_HANDLE_VALUE;
        continue;
      }
      
      int fd = _open_osfhandle((intptr_t)file_handle, _O_APPEND | _O_RDONLY);
      
      if (fd == -1) {
        if (INVALID_HANDLE_VALUE != file_handle)
          CloseHandle(file_handle);
        
        file_handle = INVALID_HANDLE_VALUE;
        swprintf_s(debug, L"failed to obtain file descriptor for file %s", files.cFileName);
        DEXIT_PROCESS(debug, 0);
        continue;
      }
      
      ibt *telemetry = parse_ibt(fd, filename);
      char *ir_buffer;
      

      size_t chars_converted;
      errno_t error;

      char vehicle_query[255];

      wchar_t vehicle_name_unicode[255];
      wchar_t track_name_unicode[255];
      wchar_t driver_idx_unicode[255];

      ir_buffer = ir_get_variable("DriverInfo:DriverCarIdx:", telemetry->session_string);

      if(FAILED(error = mbstowcs_s(&chars_converted, driver_idx_unicode,
                                   sizeof(driver_idx_unicode)/sizeof(driver_idx_unicode[0]),
                                   ir_buffer, strlen(ir_buffer)+1)))
      {
        DEXIT_PROCESS(L"mbstowcs_s failed..", GetLastError());
      }

      sprintf(vehicle_query, "DriverInfo:Drivers:CarIdx:{%d}CarScreenNameShort:", atoi(ir_buffer));

      ir_buffer = ir_get_variable(vehicle_query, telemetry->session_string);
      
      if (FAILED(error = mbstowcs_s(&chars_converted, vehicle_name_unicode,
                                    sizeof(vehicle_name_unicode)/sizeof(vehicle_name_unicode[0]),
                                    ir_buffer, strlen(ir_buffer)+1)))
      {
        DEXIT_PROCESS(L"mbstowcs_s failed..", GetLastError());
      }
      
      
      if (ir_buffer == 0)
        continue;

      ir_buffer = ir_get_variable("WeekendInfo:TrackDisplayName:", telemetry->session_string);
      
      if (FAILED(error = mbstowcs_s(&chars_converted, track_name_unicode,
                                    sizeof(track_name_unicode)/sizeof(track_name_unicode[0]),
                                    ir_buffer, strlen(ir_buffer)+1)))
      {
        DEXIT_PROCESS(L"mbstowcs_s failed..", GetLastError());
      }
      
      wchar_t vehicle_path[MAX_PATH_UNICODE];
      wchar_t track_path[MAX_PATH_UNICODE];
      
      StringCchCopy(vehicle_path, MAX_PATH_UNICODE, directory);
      
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, vehicle_name_unicode);
      
      
      CreateDirectory(vehicle_path, NULL);

      StringCchCat(vehicle_path, MAX_PATH_UNICODE, L"\\");
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, track_name_unicode);
      
      CreateDirectory(vehicle_path, NULL);
      
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, L"\\");
      
      SYSTEMTIME date;
      int ret;
      ret = FileTimeToSystemTime(&files.ftCreationTime, &date);
      
      wchar_t year[5];
      _itow(date.wYear, year, 10);
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, year);
      
      CreateDirectory(vehicle_path, NULL);
      
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, L"\\");

      wchar_t *month = num_to_month(date.wMonth);
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, month);

      CreateDirectory(vehicle_path, NULL);

      StringCchCat(vehicle_path, MAX_PATH_UNICODE, L"\\");
      StringCchCat(vehicle_path, MAX_PATH_UNICODE, files.cFileName);
      
      CloseHandle(file_handle);
      MoveFile(filename, vehicle_path);
      
      free(ir_buffer);
      free(telemetry->session_string);
      free(telemetry);
    }
  }
  while (FindNextFile(search, &files) != 0);
  

}

void watch_ibt_directory(wchar_t *directory, HANDLE dw_change_handle)
{
  DWORD dw_wait_status;
  
  if (dw_change_handle == INVALID_HANDLE_VALUE) {
    DEXIT_PROCESS(L"Invalid Handle Value", GetLastError());
  }

  dw_wait_status = WaitForSingleObject(dw_change_handle, 0);

  switch (dw_wait_status)
  {
    case WAIT_OBJECT_0:
      /* A file was created, renamed, or deleted in the directory */
      sort_ibt_directory(directory);
      if (FindNextChangeNotification(dw_change_handle) == FALSE) {
        DEXIT_PROCESS(L"FindNextChangeNotification function failed.", GetLastError());
      }
      break;
      
    case WAIT_OBJECT_0 + 1:
      /* A directory was created, renamed, or deleted. */
      if (FindNextChangeNotification(dw_change_handle) == FALSE) {
        DEXIT_PROCESS(L"FindNextChangeNotification function failed.", GetLastError());
      }
      break;
      
    case WAIT_TIMEOUT:
      return;

    default:
      return;
  }
}

void setup_weekend(configuration *config)
{
  check_track(config->db);
  config->configured = true;
}
    
void loop(sqlite3 *db, ws_daemon *ws, configuration *config)
{
  watch_ibt_directory(config->telemetry_path, config->dw_change_handle);
}
