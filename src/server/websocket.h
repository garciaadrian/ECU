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

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <WinSock2.h>

#define MAX_CONNECTIONS 5

typedef struct {
  SOCKET listen;
  addrinfo *result, *ptr, hints;
} ws_daemon;

typedef struct {
  bool in_use;
  SOCKET client_socket;
  /* Sockaddr_In client_addr; */
  DWORD thread_id;
  HANDLE thread_handle;
} CLIENTS;

typedef struct _WORKER {
  BOOL in_use;
  SOCKET socket;
  unsigned int  thread_id;
  HANDLE thread_handle;
  HANDLE exit_request;
  HANDLE *thread_handles;
  HANDLE worker_exit_event;
  timeval interval;
} WORKER;

char *encode_base64();
void ws_poll(ws_daemon *ws);
unsigned __stdcall ws_start_daemon(void *p);

#endif /* WEBSOCKET_H */
