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

#ifndef BASE_H
#define BASE_H

#include "libs/sqlite3/sqlite3.h"

#define MAX_PATH_UNICODE 32767

typedef struct
{
  wchar_t path[MAX_PATH_UNICODE];
  wchar_t telemetry_path[MAX_PATH_UNICODE];
  sqlite3 *db;
  bool configured = false;
  HANDLE dw_change_handle;
  
  /* CONFIG.INI */
  const char *ip;
  bool ibt_sorting = false;
  bool headless = false;

  float angle = 0.0;
  float derate_fixed = 0.0;
  float normrate_fixed = 0.0;
} configuration;


#endif /* BASE_H */
