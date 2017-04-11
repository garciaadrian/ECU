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

#ifndef LOOP_H
#define LOOP_H

#include <wchar.h>
#include <db/sqlite.h>
#include <base/debug.h>
#include <server/websocket.h>
#include "ir_helper.h"

int create_var_row();
void check_track(sqlite3 *db);
void setup_weekend(configuration *config); /* see https://www.sqlite.org/c3ref/c_abort.html for return codes */
void sort_ibt_directory(wchar_t *directory);

/* typedef struct */
/* { */
/*   irsdk_header header; */
/*   irsdk_diskSubHeader disk_subheader; */
/*   irsdk_varHeader *var_headers; */
/*   char *var_buf; */
/*   char *session_string; */
/* } ibt; */

#endif /* LOOP_H */
