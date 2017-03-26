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
#include <stdint.h>

#define MAX_CONNECTIONS 5
#define DATA_BUFSIZE 8192
#define WEBSOC_KEYSIZE 256

#define APPLICATION_DATA_SIZE 1024
#define THREAD_POOL_SIZE 5
#define MAX_FRAMES 5

typedef struct {
  CRITICAL_SECTION cs;
  bool initialized = false;
} CRITICAL_SECTION_WRAPPER;

/* TODO: use stdints for guaranteed sizes */
typedef struct {
  bool in_use = 0;
  unsigned char opcode;         /* 4 bits */
  bool mask;                    /* 1 bit */
  unsigned long payload_length; /* 7 bits, 7+16 bits, or 7+64 bits*/
  unsigned int masking_key;     /* 4 bytes */
  unsigned char application_data[APPLICATION_DATA_SIZE]; /* Extension data + Application Data */  
} FRAME;

typedef struct {
  char key[WEBSOC_KEYSIZE] = {0};
} REQUEST;

typedef struct _WORKER {
  BOOL in_use;
  SOCKET socket;
  unsigned int  thread_id;
  HANDLE thread_handle;
  HANDLE exit_request;
  HANDLE *thread_handles;
  HANDLE worker_exit_event;
  timeval interval;
  char recv_buffer[DATA_BUFSIZE] = {0};
  REQUEST req;
  FRAME frames[MAX_FRAMES];
} WORKER;

WORKER *get_active_sockets();
void send_frame(WORKER *client, wchar_t *text, int length);
unsigned __stdcall ws_start_daemon(void *p);

#endif /* WEBSOCKET_H */
