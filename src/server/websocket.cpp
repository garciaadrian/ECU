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
#include <Wincrypt.h>
#include <process.h>

#define PORT "26162"

#define THREAD_POOL_SIZE 5

static short worker_count = 0;
static CRITICAL_SECTION thread_update;

unsigned char *hash_secws_key(unsigned char *key, int size)
{
  HCRYPTPROV hProv;
  HCRYPTHASH hHash;

  DWORD cbHash;
  
  if (!CryptAcquireContext(&hProv, NULL, NULL,
                           PROV_RSA_FULL,
                           CRYPT_VERIFYCONTEXT)) {
    DEXIT_PROCESS(L"CryptAcquireContext failed.", GetLastError());
  }

  if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {

    CryptReleaseContext(hProv, 0);
    DEXIT_PROCESS(L"CryptAcquireContext failed.", GetLastError());
  }

  if (!CryptHashData(hHash, key, size, NULL)) {
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    DEXIT_PROCESS(L"CryptAcquireContext failed.", GetLastError());
  }

  cbHash = 20;

  unsigned char *output = (unsigned char*)malloc(size+1);
  SecureZeroMemory(output, size+1);
  
  if (CryptGetHashParam(hHash, HP_HASHVAL, output, &cbHash, NULL)) {
    
  }
  
  return output;
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

wchar_t *encode_base64(char *message)
{
  DWORD size = 256;

  /*  CryptBinaryToString((unsigned char*)message, strlen(message),
      CRYPT_STRING_BASE64, NULL, &size); */

  wchar_t *output = (wchar_t*)malloc(256);

  CryptBinaryToString((unsigned char*)message, strlen(message),
                      CRYPT_STRING_BASE64, output, &size);
  return output;
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

/* validate_connection? */
int ws_open_handshake(WORKER *client)
{
  static const char request_line[] = "GET / HTTP/1.1\r\n";
  static const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  if (strncmp(request_line, client->recv_buffer, strlen(request_line)) != 0) {
    LOGF(WARNING, "(%s) Invalid HTTP request made", client->thread_id);
    remove_thread_handle(client->thread_handle, client->thread_handles,
                         client->exit_request);
    worker_cleanup(client);
    return E_ABORT;
  }
  
  char *index = strstr(client->recv_buffer, "Sec-WebSocket-Key:");

  if (index == NULL) {
    LOGF(WARNING, "(%s) Invalid HTTP request made", client->thread_id);
    remove_thread_handle(client->thread_handle, client->thread_handles,
                         client->exit_request);
    worker_cleanup(client);
    return E_ABORT;
  }
  
  for (int x = 0,  i = strlen("Sec-WebSocket-Key:"); i < strlen(index); i++) {
    if (index[i] == 0) 
      continue;
    if (index[i] == 32) /* space char */
      continue;
    if (index[i] == '\r') {
      client->req.key[x] = '\0';
      break;
    }
    client->req.key[x] = index[i];
    x++;
  }

  unsigned char *resp_key;
  wchar_t *encoded_key;
  
  strcat(client->req.key, guid);
  
  LOGF(DEBUG, "Key+GUID is %s", client->req.key);
  resp_key = hash_secws_key((unsigned char*)client->req.key, strlen(client->req.key));
  
  char sha1_str[256];
  
  for (int i = 0; i < 20; i++) {
    sprintf(&sha1_str[i*2], "%02x", resp_key[i]);
  }

  LOGF(DEBUG, "SHA1: %s", sha1_str);
  
  encoded_key = encode_base64((char*)resp_key);
  LOGF(DEBUG, "base64: %S", encoded_key);
  
  char response[512] = {0};
  wcstombs(response, encoded_key, 512);
  
  sprintf(response,
          "HTTP/1.1 101 Switching Protocols\r\n"
          "Upgrade: websocket\r\n"
          "Connection: Upgrade\r\n"
          "Sec-Websocket-Accept: %S\r\n\r\n", encoded_key);
  
  int ret = send(client->socket, response, strlen(response), NULL);

  if (ret == SOCKET_ERROR) {
    LOGF(WARNING, "(%d) send() failed with error %d",
         client->thread_id, WSAGetLastError());
    
    remove_thread_handle(client->thread_handle, client->thread_handles,
                         client->exit_request);
    
    worker_cleanup(client);
    return E_ABORT;
  }
  
  free(encoded_key);
  free(resp_key);

  return EXIT_SUCCESS;
  
}

unsigned __stdcall ws_worker(void *p)
{
  WORKER* client = (WORKER*)p;
  fd_set fdread = {0};
  int ret = 0;
  int recv_ret = 0;

  SecureZeroMemory(&client->recv_buffer, DATA_BUFSIZE);

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
        ret = recv(client->socket, client->recv_buffer, DATA_BUFSIZE, NULL);
        if (ret > 0) {
          LOGF(DEBUG, "(%d) Bytes Received: %d", client->thread_id, ret);

          if (!client->in_use) {
            int hs = ws_open_handshake(client);
            SecureZeroMemory(client->recv_buffer, DATA_BUFSIZE);
            
            if (hs == E_ABORT) {
              return E_ABORT;
            }
          }          
        }
        
        /* Peer has disconnected */
        if (ret == -1 || ret == 0) {
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
      thread->in_use = false;
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
