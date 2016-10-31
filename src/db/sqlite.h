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

#ifndef SQLITE_H
#define SQLITE_H

#include <stdio.h>
#include <iostream>
#include "libs/sqlite3/sqlite3.h"
#include "base/debug.h"
#include <Shlwapi.h>
#include <base/base.h>

int handle_sql_rc(int rc, char *sql, sqlite3 *db, bool step,
                  bool warning_as_error, bool select);

void init_db(configuration *config);

#endif
