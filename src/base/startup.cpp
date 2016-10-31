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

configuration *ecu_init()
{
  configuration *config = (configuration *)malloc(sizeof(configuration));
  config->configured = false;
  static char debug[128];
  static wchar_t debug_wide[246];

  if (GetModuleFileName(NULL, config->path, MAX_PATH_UNICODE) == 0)
    DEXIT_PROCESS();
  size_t len = wcsnlen_s(config->path, MAX_PATH_UNICODE);
  
  /* TODO: add support for changing executable name */
  config->path[len - 7] = '\0'; /* ECU.exe */
  
  // Don't execute anything until iRacing is running
  DEBUG_OUTA("[STARTUP]: Init ECU\n", debug);
  init_db(config);
  return config;
}
