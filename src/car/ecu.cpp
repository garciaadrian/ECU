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

#include <stdlib.h>
#include <fcntl.h>
#include <Windows.h>
#include <json.hpp>
#include <Shlwapi.h>
#include <Strsafe.h>
#include <Shlobj.h>
#include <io.h>
#include <base/debug.h>
#include <car/ecu.h>
#include <car/ers.h>
#include <WinUser.h>
#include <server/websocket.h>

using json = nlohmann::json;

#define TIMEOUT 18

// TODO: Send keys only when iRacing is the active window
void input_send(float steps, int key)
{
  INPUT ip = {0};
  for (int i = 0; i < steps; i++) {
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = key;
    SendInput(1, &ip, sizeof(INPUT));
    ip.ki.wVk = VK_MENU;
    SendInput(1, &ip, sizeof(INPUT));
    Sleep(30);
  }
  ip.ki.wVk = VK_MENU;
  ip.ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(1, &ip, sizeof(INPUT));

  ip.ki.wVk = key;
  SendInput(1, &ip, sizeof(INPUT));
}

int var_offset(irsdk_varHeader *varHeaders, char *name, int count)
{
  for (int i = 0; i <= count; i++) {
    if (strcmp(name, varHeaders[i].name) == 0)
      return varHeaders[i].offset;
  }
}

void del_telemetry(ECU *settings)
{
  for (int i = 0; i < settings->handles.size(); i++) {
    HANDLE h = settings->handles.at(i);
    std::wstring file_path = settings->file_paths.at(i);
    
    CloseHandle(h);
    DeleteFile(file_path.c_str());
    settings->handles.pop_back();
    settings->file_paths.pop_back();
  }
}

ibt *last_linebuf(intptr_t fd, ECU *settings)
{
  FILE *file = 0;
  if (fd == -1)
    LOGF(FATAL, "Invalid file descriptor");
  
  file = _fdopen(fd, "r");

  int ret = 0;
  int len = 0;
  ibt *telemetry = (ibt*)malloc(sizeof(ibt));

  ret = fread(&telemetry->header, 1, sizeof(irsdk_header), file);
  if (ret == 0) {
    LOGF(WARNING, "ret returned 0, fd is :%d", fd);
  }
  
  telemetry->var_headers =
      (irsdk_varHeader*)malloc(telemetry->header.numVars * sizeof(irsdk_varHeader));
  
  telemetry->var_buf = (char*)malloc(telemetry->header.bufLen);

  fseek(file, telemetry->header.varHeaderOffset, SEEK_SET);
  len = sizeof(irsdk_varHeader)*telemetry->header.numVars;

  // TODO: Use the last line buffer for best accurate info.
  ret = fread(telemetry->var_headers, 1, len, file);
  fseek(file, telemetry->header.varBuf[0].bufOffset, SEEK_SET);

  len = telemetry->header.bufLen;
  ret = fread(telemetry->var_buf, 1, len, file);

  del_telemetry(settings);
  return telemetry;
}

intptr_t loop_files(HANDLE search,
                    WIN32_FIND_DATA files,
                    wchar_t *directory,
                    ECU *settings)
{
  wchar_t extension[4];
  wchar_t file_path[MAX_PATH_UNICODE];

  do
  {
    // Get file extension
    int len = wcsnlen(files.cFileName, MAX_PATH);
    wcsncpy(extension, files.cFileName + (len - 3), 4);

    if (lstrcmp(extension, L"ibt") == 0) {
        StringCchCopy(file_path, MAX_PATH_UNICODE, directory);
        StringCchCat(file_path, MAX_PATH_UNICODE, files.cFileName);

        HANDLE file = CreateFile(file_path, GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ, NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_RANDOM_ACCESS, NULL);

        // If file is open in another processs
        if (file == INVALID_HANDLE_VALUE && GetLastError() == 32) {
          LOGF(WARNING, "Unable to get file handle for %S", file_path);
          LOGF(WARNING, "Probably being used by iRacing. restarting telemetry..");
          irsdk_broadcastMsg(irsdk_BroadcastTelemCommand,
                             irsdk_TelemCommand_Stop, 0, 0);
          
          // Give iRacing time to release the file handle
          Sleep(200);
          irsdk_broadcastMsg(irsdk_BroadcastTelemCommand,
                             irsdk_TelemCommand_Start, 0, 0);
          
          // iRacing starts writing to disk 2.5 seconds after
          // telemetry is turned on.
          Sleep(2500);
          HANDLE file = CreateFile(file_path, GENERIC_READ, NULL, NULL,
                                 OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
          if (file == INVALID_HANDLE_VALUE) {
            LOGF(WARNING,
                 "iRacing still has a open handle to the old telemetry file! GLE: %d",
                 GetLastError());
            return -1;
          }

          int fd = _open_osfhandle((intptr_t)file, _O_APPEND | _O_RDONLY);
          
          if (fd == -1) {
            CloseHandle(file);
            LOGF(WARNING, "Unable to obtain file descriptor for %s", files.cFileName);
            return -1;
          }
          settings->handles.push_back(file);
          settings->file_paths.push_back(std::wstring(file_path));
          return fd;
        }
        
        CloseHandle(file);
    }
  }

  while (FindNextFile(search, &files) != 0);

  // If iRacing is not writing to a file
  return 0;
}

intptr_t find_latest_file(wchar_t *directory, ECU *settings)
{
  WIN32_FIND_DATA files;
  intptr_t fd;
  HANDLE search;
  wchar_t telemetry_path[MAX_PATH_UNICODE];
  wchar_t file_path[MAX_PATH_UNICODE];
  wchar_t extension[4];

  // TODO: add the wildcard to the directory during process startup 
  StringCchCopy(telemetry_path, MAX_PATH_UNICODE, directory);
  StringCchCat(telemetry_path, MAX_PATH_UNICODE, L"*");

  if (!PathFileExists(directory))
    // Path length limit is 248 characters inc. null terminator
    SHCreateDirectory(NULL, directory);

  search = FindFirstFile(telemetry_path, &files);

  if (search == INVALID_HANDLE_VALUE)
    LOGF(FATAL, "FindFirstFile returned INVALID HANDLE. Possibly empty telemetry folder");
  
  fd = loop_files(search, files, directory, settings);
  
  // If disk telemetry is not running
  if (fd == 0) {
    irsdk_broadcastMsg(irsdk_BroadcastTelemCommand, irsdk_TelemCommand_Start, 0, 0);
    int timeout = 250;
    
    // Give iRacing time to write to file
    Sleep(timeout);
    search = FindFirstFile(telemetry_path, &files);
    fd = loop_files(search, files, directory, settings);
    if (fd == 0)
      LOGF(FATAL, "iRacing did not write to new telemetry file. timeout was %d", timeout);
  }

  return fd;
}

int calibrate_ecu(ECU *settings)
{
  // Check if telemetry is writing to a file.
  // If telemetry is off when the car hits the track
  // then turn it on and off quickly. Then get all your
  // data then delete the telemetry file and turn on telemetry
  // for normal use. TODO: measure the time gap of turning
  // telemetry on and off.

  // Find the newest file and check if it's being used by iRacing
  LOGF(WARNING, "Car on track. Calibrating ECU");
  intptr_t newest = find_latest_file(settings->config->telemetry_path, settings);

  if (newest != -1)
    settings->telemetry = last_linebuf(newest, settings);
  
  else
    LOGF(FATAL, "File descriptor error. exiting");

  return 1;
}

// TODO: Change settings to data
void get_var(ECU *settings, char *variable, char *destination)
{
  int offset = irsdk_varNameToOffset(variable);
  const irsdk_varHeader *rec = irsdk_getVarHeaderEntry(offset);
  
  *destination = *(char*)(settings->data + offset);
}

void get_var(ECU *settings, char *variable, int *destination)
{
  int offset = irsdk_varNameToOffset(variable);
  const irsdk_varHeader *rec = irsdk_getVarHeaderEntry(offset);
  
  *destination = *(int*)(settings->data + offset);
}

void get_var(ECU *settings, char *variable, float *destination)
{
  int offset = irsdk_varNameToOffset(variable);
  const irsdk_varHeader *rec = irsdk_getVarHeaderEntry(offset);
  
  *destination = *(float*)(settings->data + offset);
}

void get_var(ECU *settings, char *variable, double *destination)
{
  int offset = irsdk_varNameToOffset(variable);
  const irsdk_varHeader *rec = irsdk_getVarHeaderEntry(offset);
  
  *destination = *(double*)(settings->data + offset);
}

int on_track(char *data, const irsdk_header *header)
{
  int IsOnTrack = irsdk_varNameToOffset("IsOnTrack");
  bool ret = *(bool *)(data + IsOnTrack);
  return ret;
}

void initData(const irsdk_header *header, char *data, int num_data)
{
  if (data)
    free(data);
  num_data = header->bufLen;
  data = (char*)malloc(num_data);
}

void shutdown_ecu(ECU *settings)
{
  LOGF(WARNING, "Shutting down ECU");
  
  // TODO: Readjust ERS deployment?

  if (settings->telemetry->var_headers != NULL)
    free(settings->telemetry->var_headers);

  if (settings->telemetry->var_buf != NULL)
    free(settings->telemetry->var_buf);

  if (settings->telemetry != NULL)
    free(settings->telemetry);

  if (settings != NULL)
    free(settings);
}

unsigned __stdcall start_ecu(void *p)
{
  ECU *settings = (ECU*)p;
  ws_event *events = settings->events;
  bool init = false;
  int missed = 0;

  // int ret = WaitForSingleObject(events->connection_event, INFINITE);

  // Use irsdk_connected() ?
  while (!init) {
    if (irsdkClient::instance().waitForData(1000)) {
      // Check if we're in replay mode by testing 
      // if the physics is running on some channels
      LOGF(DEBUG, "iRacing is online");
      json j2 = {
        {"status", 1}
      };
      
      send_json(j2.dump());
      init = true;
    }
  }

  // Test irsdk connection failures
  int tries = 500;
  
  while (tries > 0) {
    if (irsdkClient::instance().waitForData(TIMEOUT)) {}
    else
      missed++;
    tries--;
  }
  
  LOGF(DEBUG, "irsdk: timeout is %d ms", TIMEOUT);
  LOGF(DEBUG, "irsdk: %d missed connections", missed);

  int num_data = 0;
  const irsdk_header *header = irsdk_getHeader();
  
  while (true) {
    int ret = WaitForSingleObject(events->exit_event, NULL);
    
    if (ret == WAIT_OBJECT_0) {
      shutdown_ecu(settings);
      if (settings->data)
        free(settings->data);
      return 1;
    }
    
    if (header) {
      if (!settings->data || num_data != header->bufLen) {
        if (settings->data)
          free(settings->data);
        num_data = header->bufLen;
        settings->data = (char*)malloc(num_data);
        irsdk_waitForDataReady(TIMEOUT, settings->data);
      }
      else {
        if (!irsdk_isConnected()) {
          LOGF(DEBUG, "iRacing just went offline!");
          settings->calibrated = false;
          json offline = {
            {"status", 0}
          };
          
          if (settings->data)
            free(settings->data);
          
          send_json(offline.dump());
          shutdown_ecu(settings);
          break;
        }
        irsdk_waitForDataReady(TIMEOUT, settings->data);
        
        // Copy all vars to a buffer?
        if (on_track(settings->data, header)) {
          if (!settings->calibrated) {
            settings->hWnd = GetForegroundWindow();
            calibrate_ecu(settings);
            settings->calibrated = true;
          }
          ers(settings);
        }
        else {
          if (settings->calibrated)
            LOGF(WARNING, "Car exited the track");
          settings->calibrated = false;
        }
      }
    }
  }
  
  return 1;
}
