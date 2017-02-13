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
#include <process.h>

#define PORT "26162"

#define THREAD_POOL_SIZE 5
#define DATA_BUFSIZE 8192

static short worker_count = 0;
static CRITICAL_SECTION thread_update;

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

}

void check_peer_connected()
{

}

void remove_thread_handle(HANDLE handle, HANDLE *handle_array, HANDLE exit)
{
  /* if exit is requested we should not remove any handles */
  int ret = WaitForSingleObject(exit, NULL);

  if (ret == WAIT_OBJECT_0)
    return;

  EnterCriticalSection(&thread_update);
    
  for (int i = 0; i < worker_count; i++) {
    if (handle_array[i] == handle) {
      /* this can be optimized by checking if we're the last/first thread and
       * skipping memmove */
      CloseHandle(handle_array[i]);
      /* or memcpy? */
      memmove(&handle_array[i], &handle_array[worker_count], sizeof(HANDLE));
      SecureZeroMemory(&handle_array[worker_count], sizeof(HANDLE));
      break;
    }
  }
  
  LeaveCriticalSection(&thread_update);
}

void worker_cleanup(WORKER *client)
{
  EnterCriticalSection(&thread_update);
  
  LOGF(DEBUG, "closing worker thread %d", client->thread_id);
  closesocket(client->socket);
  SetEvent(client->worker_exit_event);
  free(client);
  worker_count--;

  LeaveCriticalSection(&thread_update);
}

unsigned __stdcall ws_worker(void *p)
{
  WORKER* client = (WORKER*)p;
  fd_set fdread = {0};
  int ret = 0;
  int recv_ret = 0;
  char recv_buffer[DATA_BUFSIZE];

  while (true) {
    FD_ZERO(&fdread);
    FD_SET(client->socket, &fdread);

    if ((recv_ret = select(0, &fdread, NULL, NULL, &client->interval)) == SOCKET_ERROR) {
      LOGF(WARNING, "(%d) select() returned with error %d",
           client->thread_id, WSAGetLastError());
      LOGF(WARNING, "closing worker thread %d", client->thread_id);
      remove_thread_handle(client->thread_handle, client->thread_handles,
                           client->exit_request);
      worker_cleanup(client);
      return E_ABORT;
    }

    if (recv_ret) {
      if (FD_ISSET(client->socket, &fdread)) {
        /* read event occured */
        ret = recv(client->socket, recv_buffer, DATA_BUFSIZE, NULL);
        if (ret > 0) {
          LOGF(DEBUG, "(%d) Bytes Received: %d", client->thread_id, ret);
          ret = send(client->socket, recv_buffer, ret, NULL);
          
          if (ret == SOCKET_ERROR) {
            LOGF(WARNING, "(%d) send() failed with error %d",
                 client->thread_id, WSAGetLastError());
            remove_thread_handle(client->thread_handle, client->thread_handles,
                                 client->exit_request);
            worker_cleanup(client);
            return E_ABORT;
          }
        }
        /* Peer has disconnected */
        if (ret == -1) {
          LOGF(WARNING, "Peer has disconnected");
          remove_thread_handle(client->thread_handle, client->thread_handles,
                               client->exit_request);
          worker_cleanup(client);
          return EXIT_SUCCESS;
        }
      }
    }

    else
      FD_SET(client->socket, &fdread);
    
    ret = WaitForSingleObject(client->exit_request, NULL);
  
    if (ret == WAIT_OBJECT_0) {
      worker_cleanup(client);
      return EXIT_SUCCESS;
    }
  }
    
  
  /* If client has disconnected AND there is not request to exit,
   * then compress handle array, a worker thread should not modify
   * handle array since ws_start_deamon is waiting on the array */
}

unsigned __stdcall ws_start_daemon(void *p)
{
  InitializeCriticalSection(&thread_update);
  HANDLE exit_event = (HANDLE)p;
  CLIENTS clients[MAX_CONNECTIONS] = {0};
  WSADATA wsad;
  int ret;
  
  ret = WSAStartup(MAKEWORD(2,2), &wsad);
  
  if (ret != 0) {
    LOGF(FATAL, "WSAStartup failed: %d", ret);
    return E_ABORT;
  }

  struct addrinfo *result = NULL, *ptr = NULL, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  ret = getaddrinfo(NULL, PORT, &hints, &result);
  if (ret != 0) {
    LOGF(FATAL, "getaddrinfo failed: %d", ret);
    WSACleanup();
    return E_ABORT;
  }

  SOCKET listen_socket = INVALID_SOCKET;
  
  listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listen_socket == INVALID_SOCKET) {
    LOGF(FATAL, "Error at socket(): %ld", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return E_ABORT;
  }

  ret = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
  if (ret == SOCKET_ERROR) {
    LOGF(FATAL, "bind failed with error: %d", WSAGetLastError());
    freeaddrinfo(result);
    closesocket(listen_socket);
    WSACleanup();
    return E_ABORT;
  }
  
  freeaddrinfo(result);

  ULONG non_block = 1;
  if (ioctlsocket(listen_socket, FIONBIO, &non_block) == SOCKET_ERROR) {
    LOGF(FATAL, "ioctlsocket(FIONBIO) failed with error %d", GetLastError());
    closesocket(listen_socket);
    WSACleanup();
    return E_ABORT;
  }

  timeval interval = {0};
  interval.tv_sec = 2;
  
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(listen_socket, &readfds);

  int total = 0;

  if (listen(listen_socket, 5) == SOCKET_ERROR) {
    LOGF(FATAL, "Listen failed with error: %ld", WSAGetLastError());
    closesocket(listen_socket);
    WSACleanup();
    return E_ABORT;
  }

  HANDLE *thread_handles = (HANDLE*)malloc(sizeof(HANDLE) * THREAD_POOL_SIZE);
  HANDLE worker_exit_event = CreateEvent(NULL, TRUE, FALSE, NULL);

  SecureZeroMemory(thread_handles, sizeof(HANDLE) * THREAD_POOL_SIZE);

  while (true) {
    if ((total = select(0, &readfds, NULL, NULL, &interval)) == SOCKET_ERROR) {
      LOGF(FATAL, "select() returned with error %d", WSAGetLastError());
      return E_ABORT;
    }

    if (FD_ISSET(listen_socket, &readfds)) {
      
      if (worker_count == THREAD_POOL_SIZE) {
        /* do something more robust pls */
        FD_ZERO(&readfds);
        FD_SET(listen_socket, &readfds);
        LOGF(WARNING, "Connection attempt failed. Maximum connections reached!");
        ResetEvent(worker_exit_event);
        LOGF(WARNING, "Waiting for a worker to exit");
        WaitForSingleObject(worker_exit_event, INFINITE);
        ResetEvent(worker_exit_event);
        continue;
      }
      
      LOGF(WARNING, "there is an arriving connection");
      SOCKET accept_socket = INVALID_SOCKET;

      accept_socket = accept(listen_socket, NULL, NULL);

      /* Check for WSAEWOULDBLOCK? */
      if (accept_socket == INVALID_SOCKET) { 
        LOGF(FATAL, "accept() failed with error %d", GetLastError());
        return E_ABORT;
      }

      ret = ioctlsocket(accept_socket, FIONBIO, &non_block);

      if (ret == SOCKET_ERROR) {
        LOGF(FATAL, "ioctlsocket(FIONBIO) failed with error %d", GetLastError());
        return E_ABORT;
      }

      EnterCriticalSection(&thread_update);

      WORKER *thread = (WORKER*)malloc(sizeof(WORKER));
      thread->socket = accept_socket;
      thread->exit_request = exit_event;
      thread->thread_handles = thread_handles;
      thread->in_use = true;
      thread->interval.tv_sec = 20;
      thread->interval.tv_usec = 0;
      thread->worker_exit_event = worker_exit_event;
      
      
      thread_handles[worker_count] =
          (HANDLE)_beginthreadex(NULL, NULL, ws_worker,
                                 (void*)thread, NULL,
                                 &thread->thread_id);
      
      /* Pause thread so it receives thread handle before starting? */
      thread->thread_handle = thread_handles[worker_count];
      
      SetThreadName("Websocket Worker Thread", thread->thread_id);
      worker_count++;
      LeaveCriticalSection(&thread_update);
      
    }

    else
      FD_SET(listen_socket, &readfds);

    ret = WaitForSingleObject(exit_event, NULL);

    if (ret == WAIT_OBJECT_0) {
      WaitForMultipleObjects(worker_count, thread_handles, true, INFINITE);
      LOGF(DEBUG, "All workers exited. Closing Websocket thread manager");
      closesocket(listen_socket);
      WSACleanup();
      DeleteCriticalSection(&thread_update);
      return EXIT_SUCCESS;
    }
  
  }  
}
