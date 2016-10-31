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
#include <base/websocket.h>
#include "ir_helper.h"

int create_var_row();
void check_track(sqlite3 *db);
void setup_weekend(configuration *config); /* see https://www.sqlite.org/c3ref/c_abort.html for return codes */
void loop(sqlite3* db, ws_daemon *ws);

#endif /* LOOP_H */
