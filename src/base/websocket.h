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

typedef struct {
  SOCKET socket_array[WSA_MAXIMUM_WAIT_EVENTS];
  WSAEVENT event_array[WSA_MAXIMUM_WAIT_EVENTS];
  SOCKADDR_IN internet_addr;
  SOCKET accept, listen;
  DWORD event_total;
  DWORD index, i;
  _WSANETWORKEVENTS network_events;
  WSAEVENT new_event;
  char recv_buffer[4096];
  char send_buffer[4096];
  WSADATA wsa_data;
  WORD wsa_version;
  fd_set fd_read;
  fd_set fd_write;
} ws_daemon;

char *encode_base64();
ws_daemon *ws_start_daemon(ws_daemon *ws);
void ws_poll(ws_daemon *ws);

#endif /* WEBSOCKET_H */
