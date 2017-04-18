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

#include "sqlite.h"


// TODO:
// function can be refactored so caller does not need
// to pass return code or step variable. By using
// sqlite3_errcode() to get the most recent return code

int handle_sql_rc(int rc, char *sql, sqlite3 *db, bool step,
                  bool warning_as_error, bool select)
{
  
  switch (step) {
    case true: {
      if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
        LOGF(WARNING,
             "[DATABASE]: sqlite3_step did not return SQLITE_ROW. RETURN CODE: %d\n"
             "[DATABASE]: RETURN CODE: %s\n"
             "[DATABASE]: ERROR: %s\n",
             rc, sqlite3_errstr(rc), sqlite3_errmsg(db));
        if (warning_as_error)
          DEXIT_PROCESS(L"sqlite3_step failed", GetLastError());
        return 0;
      }
      if (rc == SQLITE_DONE && select) {
        LOGF(DEBUG,
             "[DATABASE]: No rows for sql statement:\n"
             "[DATABASE]: %s\n", sql);
        return 0;
      }
      return 1;
    }
    case false: {
      if (rc != SQLITE_OK) {
        LOGF(DEBUG,
             "[DATABASE]: sqlite3_prepare_v2 did not return SQLITE_OK. RETURN CODE: %d\n"
             "[DATABASE]: RETURN CODE: %s\n"
             "[DATABASE]: ERROR: %s\n",
             rc, sqlite3_errstr(rc), sqlite3_errmsg(db));
        if (warning_as_error)
          DEXIT_PROCESS(L"sqlite3_prepare_v2 failed.", GetLastError());
        return 0;
      }
      return 1;
    }
  }
}

void init_db(configuration *config) {
  static char debug[2048];
  
  int rc = 0;
  char *zErrMsg = 0;
  sqlite3_stmt *sql_stmt;

  wchar_t db_path[MAX_PATH];
  char db_path_mb[MAX_PATH];
  
  wcscpy_s(db_path, MAX_PATH, config->path);
  wcscat_s(db_path, MAX_PATH, L"database.db");

  /* If database is created it needs sqlite to run the schema */
  if (!PathFileExists(db_path)) {
    LOGF(DEBUG, "database.db not found. creating..");
    CreateFile(db_path, GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                NULL);
    rc = WideCharToMultiByte(CP_UTF8, NULL, &db_path[0], -1, &db_path_mb[0],
                             sizeof(db_path_mb)/sizeof(db_path_mb[0]),
                             NULL, NULL);
    if (rc == 0) {
      DEXIT_PROCESS(L"WideCharToMultiByte failed.", GetLastError());
    }
    
    rc = sqlite3_open(db_path_mb, &(config->db));
    if (rc) {
      LOGF(WARNING, "Can't open database %s", db_path_mb);
      sqlite3_close(config->db);
      DEXIT_PROCESS(L"Can't open database", GetLastError());
    }    
  }

  else {
    rc = WideCharToMultiByte(CP_UTF8, NULL, &db_path[0], -1, &db_path_mb[0],
                             sizeof(db_path_mb)/sizeof(db_path_mb[0]),
                             NULL, NULL);
    if (rc == 0) {
      DEXIT_PROCESS(L"WideCharToMultiByte failed.", GetLastError());
    }
    rc = sqlite3_open(db_path_mb, &(config->db));
    if (rc) {
      LOGF(WARNING, "Can't open database %s", db_path_mb);
      sqlite3_close(config->db);
      DEXIT_PROCESS(L"Can't open database", GetLastError());
    }
  }
}
