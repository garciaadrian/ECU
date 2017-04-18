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

#include "ir_helper.h"
#include <stdlib.h>


//  TODO:
//  Refactor this function so caller doesn't need to create a new
//  variable a strncpy_s the return value to it.

char *ir_get_variable(char *variable, const char *session_string)
{
  const char *yaml_iracing_buffer;
  char *ir_buffer;
  
  // As of SDK 1.09, session strings are atleast 50 kilabytes
  // TODO: get strlen_s of session string, use for malloc instead of guessing
  ir_buffer = (char *)malloc(60000);
  int cutoff;

  if (parseYaml(session_string, variable, &yaml_iracing_buffer, &cutoff)) {
    strncpy_s(ir_buffer, 60000, yaml_iracing_buffer, cutoff);
    ir_buffer[cutoff] = '\0';
    return ir_buffer;
  }
  
}
