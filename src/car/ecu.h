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

#ifndef ECU_H
#define ECU_H

#include <server/websocket.h>
#include <base/base.h>
#include <irsdk/irsdk_client.h>
#include <irsdk/irsdk_defines.h>
#include <vector>

typedef struct _ibt {
  irsdk_header header;
  irsdk_diskSubHeader disk_subheader;
  irsdk_varHeader *var_headers;
  char *var_buf;
  char *session_string;
} ibt;

typedef struct _ECU {
  bool calibrated = false;
  ws_event *events = NULL;
  configuration *config = NULL;
  ibt *telemetry = NULL;
  HWND hWnd;
  char *data;
  std::vector<HANDLE> handles;
  std::vector<std::wstring> file_paths; 
} ECU;



unsigned __stdcall start_ecu(void *p);
void input_send(float steps, int key);

void get_var(ECU *settings, char *variable, char *destination);
void get_var(ECU *settings, char *variable, int *destination);
void get_var(ECU *settings, char *variable, float *destination);
void get_var(ECU *settings, char *variable, double *destination);

#endif // ECU_H
