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

void check_track(sqlite3 *db)
{
  static char buffer[512];
  static char sql[512];
  sqlite3_stmt *sql_stmt = {0};
  int rc = 0;

  char *ir_buffer;
  char track[255];

  if (irsdkClient::instance().waitForData(16)) {
    ir_buffer = ir_get_variable("WeekendInfo:TrackDisplayName:");
    strncpy_s(track, ir_buffer, _TRUNCATE);
    sprintf_s(sql, "SELECT * FROM tracks WHERE name='%s'", track);
    
    rc = sqlite3_prepare_v2(db, sql, sizeof(sql), &sql_stmt, NULL);
    handle_sql_rc(rc, sql, db, false, false, true);
    
    rc = sqlite3_step(sql_stmt);
    DEBUG_OUTA("[STARTUP]: Checking if current track is on the database\n", buffer);
        
    if (!handle_sql_rc(rc, sql, db, true, true, true)) {
      
      DEBUG_OUTA("[STARTUP]: Entry for track '%s' was not found in table `tracks`\n"
                 "[STARTUP]: Creating entry in table `tracks` for '%s'\n",
                 buffer, track, track);
      sprintf_s(sql, "INSERT INTO tracks(name) VALUES('%s')", track);

      rc = sqlite3_prepare_v2(db, sql, sizeof(sql), &sql_stmt, NULL);
      handle_sql_rc(rc, sql, db, false, true, false);
      rc = sqlite3_step(sql_stmt);
      handle_sql_rc(rc, sql, db, true, true, false);
      
    } else {
      DEBUG_OUTA("[STARTUP]: Track %s found in database\n", buffer, track);
      const unsigned char *sql_result = sqlite3_column_text(sql_stmt, 1);
    }
  }
}

void setup_weekend(configuration *config)
{
  check_track(config->db);
  config->configured = true;
}
    
void loop(sqlite3 *db, ws_daemon *ws)
{
  ws_poll(ws);
}
