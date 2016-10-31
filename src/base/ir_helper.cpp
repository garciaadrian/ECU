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

/*
 * TODO:
 * Refactor this function so caller doesn't need to create a new
 * variable a strncpy_s the return value to it.
 */
char *ir_get_variable(char *variable)
{
  char buffer[512];
  const char *yaml_iracing_buffer;
  char ir_buffer[256];
  int cutoff;

  if (parseYaml(irsdk_getSessionInfoStr(), variable, &yaml_iracing_buffer, &cutoff)) {
    strncpy_s(ir_buffer, yaml_iracing_buffer, cutoff);
    ir_buffer[cutoff] = '\0';
    return ir_buffer;
  }
  
}
