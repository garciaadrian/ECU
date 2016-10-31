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

#include "websocket.h"

#include <base/debug.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <Ws2tcpip.h>
#include <conio.h>
#include <Bcrypt.h>

char *hash_secws_key(const unsigned char *key, int size)
{
  BCRYPT_ALG_HANDLE alg_handle = {0};
  BCRYPT_HASH_HANDLE sha_handle = {0};
  DWORD hash_size = 0;
  DWORD result_size = 0;
  ULONG bytes_written;

  
  BCryptOpenAlgorithmProvider(&alg_handle, BCRYPT_SHA1_ALGORITHM,
                              NULL, BCRYPT_HASH_REUSABLE_FLAG);
  BCryptGetProperty(alg_handle, BCRYPT_OBJECT_LENGTH, (PBYTE)&hash_size,
                    sizeof(hash_size), &bytes_written, NULL);
  
  unsigned char *hash_obj = (unsigned char*)(malloc(hash_size));
  
  BCryptCreateHash(alg_handle, &sha_handle, hash_obj, hash_size,
                   NULL, NULL, BCRYPT_HASH_REUSABLE_FLAG);
  
  BCryptHashData(sha_handle, (PUCHAR)key, size, NULL);
  
  BCryptGetProperty(alg_handle, BCRYPT_HASH_LENGTH, (PBYTE)&result_size,
                    sizeof(result_size), &bytes_written, NULL);
  
  unsigned char *result = (unsigned char*)malloc(result_size);
  
  BCryptFinishHash(sha_handle, result, result_size, NULL);

  
  BCryptDestroyHash(sha_handle);
  free(hash_obj);
  BCryptCloseAlgorithmProvider(alg_handle, NULL);
  free(result);
  return "lol";
}

char lookup_base64(unsigned char value)
{
  const char characters[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+/";
  const char pad = '=';

  if (value == 101)
    return pad;
  
  for (int i = 0; i < 64; i++) {
    if (value == i)
      return characters[i];
  }
  return 100;
}

char *encode_base64()
{
  /* message in hex format */
  const char message[] = "ff";
  const char result[] =
      "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIG"
      "EgcG9pc29ub3VzIG11c2hyb29t";
  
  
  const unsigned char hex[] = "0123456789ABCDEF";
  

  int length = sizeof(message)/sizeof(message[0]);

  unsigned char *output = (unsigned char *)malloc(length/2);
  char *buffer = (char *)malloc(1);

  memcpy(buffer, &message[0], 2);
  unsigned char a = 0;
  unsigned char b = 0;
  unsigned char c = 0;
  
  /*
   *  49       27        6D
   * 
   * 01001001 00100111 01101101
   *
   * 010010 010010 011101 101101
   */
  
  for (int i = 0; i < length; i+=6) {
    memcpy(buffer, &message[i], 2);
    a = (unsigned char)strtol(buffer, (char **)0, 16);
    if (a == 0)
      break;
    memcpy(buffer, &message[i+2], 2);
    b = (unsigned char)strtol(buffer, (char **)0, 16);
    memcpy(buffer, &message[i+4], 2);
    c = (unsigned char)strtol(buffer, (char **)0, 16);

    unsigned char value1 = (a & 0xFC) >> 2;
    unsigned char value2 = ((a & 0x3) << 4) + ((b & 0xF0) >> 4);
    unsigned char value3 = (b == 0) ? 101 : ((b & 0xF) << 2) + ((c & 0xC0) >> 6);
    unsigned char value4 = (c == 0) ? 101 : c & 0x3F;

    printf("%c%c%c%c",
           lookup_base64(value1),
           lookup_base64(value2),
           lookup_base64(value3),
           lookup_base64(value4));
    
  }

  printf("\n%s\n", result);

  free(output);
  free(buffer);

  return "lol";

}

void ws_parse_header(char *request)
{

}

void ws_init_conn()
{
  
}

void ws_poll(ws_daemon *ws)
{
  static char debug[5000];
  int ret = 0;
  
  ws->index = WSAWaitForMultipleEvents(ws->event_total, ws->event_array,
                                       FALSE, 0, FALSE);
  if (ws->index == WSA_WAIT_FAILED) {
    DEBUG_OUTA("WSAWaitForMultipleEvents failed with error code %d\n",
    debug, WSAGetLastError());
  }
  ws->index = ws->index - WSA_WAIT_EVENT_0;

  for (int i = 0; i < ws->event_total; i++) {
    ws->index = WSAWaitForMultipleEvents(1, &(ws->event_array[i]), true, 0, false);

    if (ws->index == WSA_WAIT_FAILED || ws->index == WSA_WAIT_TIMEOUT) {
      DEBUG_OUTA("ws->index is %d\n"
                 "WSA_WAIT_FAILED: %d\n"
                 "WSA_WAIT_TIMEOUT: %d\n"
                 "error code %d\n",
                 debug, ws->index, WSA_WAIT_FAILED, WSA_WAIT_TIMEOUT,
                 WSAGetLastError());
      continue;
    }
    
    else {
      ws->index = i;
      ret = WSAEnumNetworkEvents(ws->socket_array[ws->index],
                                 ws->event_array[ws->index],
                                 &(ws->network_events));
      if (ret != 0) {
        printf("WSAEnumNetworkEvents failed with error code: %d\n",
               WSAGetLastError());
      }
      
      if (ws->network_events.lNetworkEvents & FD_ACCEPT) {
        if (ws->network_events.iErrorCode[FD_ACCEPT_BIT] != 0) {
          // DEBUG_OUTA("FD_ACCEPT failed with code %d, error %d\n", debug,
          //            ws->network_events.iErrorCode[FD_ACCEPT_BIT],
          //            WSAGetLastError());
          break;
        }

        ws->accept = accept(ws->socket_array[ws->index], NULL, NULL);

        if (ws->event_total > WSA_MAXIMUM_WAIT_EVENTS) {
          DEBUG_OUTA("Too many connections", debug);
          closesocket(ws->accept);
          break;
        }

        ws->new_event = WSACreateEvent();

        WSAEventSelect(ws->accept, ws->new_event,
                       FD_READ | FD_WRITE | FD_CLOSE);

        ws->event_array[ws->event_total] = ws->new_event;
        ws->socket_array[ws->event_total] = ws->accept;
        ws->event_total++;
        DEBUG_OUTA("Socket %lld connected \n", debug, ws->accept);
      }

      if (ws->network_events.lNetworkEvents & FD_READ) {
        if (ws->network_events.iErrorCode[FD_READ_BIT] != 0) {
          // DEBUG_OUTA("FD_READ failed with code %d, error %d\n", debug,
          //            ws->network_events.iErrorCode[FD_READ_BIT],
          //            WSAGetLastError());
          break;
        }
        int ret = 0;
        ret = recv(ws->socket_array[ws->index - WSA_WAIT_EVENT_0],
                   ws->recv_buffer,
                   sizeof(ws->recv_buffer)/sizeof(ws->recv_buffer[0]), 0);

        if (ret == SOCKET_ERROR) {
          DEBUG_OUTA("recv failed with error %d\n", debug,
                     WSAGetLastError());
          break;
        }

        char recv_msg[4096];
        strncpy_s(recv_msg,
                  sizeof(recv_msg)/sizeof(recv_msg[0]),
                  ws->recv_buffer, ret);

        DEBUG_OUTA("received %s from socket %lld\n", debug,
                   recv_msg,
                   ws->socket_array[ws->index - WSA_WAIT_EVENT_0]);
      }

      if (ws->network_events.lNetworkEvents & FD_WRITE) {
        if (ws->network_events.iErrorCode[FD_WRITE_BIT] != 0) {
          // DEBUG_OUTA("FD_WRITE failed with error %d\n", debug,
          //            ws->network_events.iErrorCode[FD_WRITE_BIT]);
          break;
        }
        
        int ret = 0;
        strncpy_s(ws->send_buffer,
                  (size_t)sizeof(ws->send_buffer)/sizeof(ws->send_buffer[0]),
                "Hello World", _TRUNCATE);
        ret = send(ws->socket_array[ws->index - WSA_WAIT_EVENT_0],
                   ws->send_buffer,
                   strlen(ws->send_buffer), 0); /* WATCH OUT strlen */
        
        if (ret == SOCKET_ERROR) {
          DEBUG_OUTA("send failed with error %d\n", debug,
                     WSAGetLastError());
        }

        DEBUG_OUTA("send %s to socket %lld\n", debug,
                   ws->send_buffer,
                   ws->socket_array[ws->index - WSA_WAIT_EVENT_0]);
      }

      if (ws->network_events.lNetworkEvents & FD_CLOSE) {
        if (ws->network_events.iErrorCode[FD_CLOSE_BIT] != 0) {
          DEBUG_OUTA("FD_CLOSE failed with error %d\n", debug,
                     ws->network_events.iErrorCode[FD_CLOSE_BIT]);
          break;
        }

        DEBUG_OUTA("closing socket %lld\n", debug,
                   ws->socket_array[ws->index - WSA_WAIT_EVENT_0]);
        
        closesocket(ws->socket_array[ws->index]);
        // CompressArrays(ws->event_array, ws->socket_array, &(ws->event_total));
      }
    }
  }
}

ws_daemon *ws_start_daemon(ws_daemon *ws)
{
  
  ws->wsa_version = MAKEWORD(2, 2);
  WSAStartup(ws->wsa_version, &(ws->wsa_data));
  ws->event_total = 0;
  ws->listen = socket(PF_INET, SOCK_STREAM, 0);
  ws->internet_addr.sin_family = AF_INET;
  ws->internet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  ws->internet_addr.sin_port = htons(5956);

  bind(ws->listen, (PSOCKADDR)&(ws->internet_addr), sizeof(ws->internet_addr));

  ws->new_event = WSACreateEvent();

  WSAEventSelect(ws->listen, ws->new_event, FD_ACCEPT | FD_CLOSE);
  listen(ws->listen, WSA_MAXIMUM_WAIT_EVENTS);
  
  ws->socket_array[ws->event_total] = ws->listen;
  ws->event_array[ws->event_total] = ws->new_event;
  ws->event_total++;
  
  return ws;
}
