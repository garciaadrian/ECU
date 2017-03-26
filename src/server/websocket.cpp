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
#include <json.hpp>

#define PORT "26162"

static short worker_count = 0;
static CRITICAL_SECTION_WRAPPER thread_update;
static WORKER *thread_list[THREAD_POOL_SIZE];

#define OPCODE_CONT 0x0
#define OPCODE_TEXT 0x1
#define OPCODE_BINARY 0x2
#define OPCODE_CLOSE 0x8
#define OPCODE_PING 0x9
#define OPCODE_PONG 0xA

void remove_thread_handle(HANDLE handle, HANDLE *handle_array, HANDLE exit);
void worker_cleanup(WORKER *client);

using json = nlohmann::json;

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

void check_peer_connected()
{

}

WORKER *get_active_sockets()
{
  if (!thread_update.initialized) {
    InitializeCriticalSection(&thread_update.cs);
  }
  
  EnterCriticalSection(&thread_update.cs);

  WORKER *list = (WORKER*)malloc(sizeof(WORKER) * THREAD_POOL_SIZE);
  SecureZeroMemory(list, sizeof(WORKER) * THREAD_POOL_SIZE);

  for (int i = 0; i < worker_count; i++) {
    if (thread_list[i] != 0)
      list[i] = *thread_list[i];
  }
  
  LeaveCriticalSection(&thread_update.cs);

  return list;
}

void send_frame(WORKER *client, const unsigned char opcode)
{
  if (opcode >= 0x9) {  /* PING, PONG */
    char buffer[10] = {0};

    buffer[0] = 0x80 | opcode;  /* FIN set, OPCODE PING or PONG set */
    int ret = send(client->socket, buffer, 10, NULL);
    LOGF(DEBUG, "(%d) sent %2x to peer", client->thread_id, buffer[0]);
  }
  
}

void send_frame(WORKER *client, wchar_t *text, int length)
{
  char *buffer = 0;

  int size = WideCharToMultiByte(CP_UTF8, NULL, text, -1, NULL, NULL,
                                 NULL, NULL);

  char *text_mb = (char*)malloc(size+1);
  SecureZeroMemory(text_mb, size+1);
  
  WideCharToMultiByte(CP_UTF8, NULL, text, length, text_mb, size,
                      NULL, NULL);
  int buf_size = 0;
  
  if (length < 126) {
    
    /* 2 is the size of the frame without payload data */
    buf_size = size + 2;
    
    buffer = (char*)malloc(buf_size);
    SecureZeroMemory(buffer, buf_size);
    buffer[0] = 0x81;  /* FIN set, OPCODE TEXT set */
    buffer[1] |= strlen(text_mb);
    memcpy(&buffer[2], text_mb, size);
  }

  if (length < 65535 && length > 126) {
    
    /* 4 is the size of the frame without payload data */
    /* TODO: use length or size for frame buffers */
    buf_size = length + 4;
    
    buffer = (char*)malloc(buf_size);
    SecureZeroMemory(buffer, buf_size);
    buffer[0] = 0x81;  /* FIN set, OPCODE TEXT set */
    buffer[1] = 126;   /* Next 2 bytes are payload size */
    buffer[2] = (length >> 8);
    buffer[3] = (length & 0xFF);
    memcpy(&buffer[4], text_mb, length);
  }

  if (length >= 65535) {
    /* TODO: add support for frames larger than 64k bytes */
  }

  int ret = send(client->socket, buffer, buf_size, NULL);
}

void ws_close_handshake(WORKER *client)
{
  LOGF(DEBUG, "(%d) received close frame");
  send_frame(client, OPCODE_CLOSE);

  remove_thread_handle(client->thread_handle, client->thread_handles,
                       client->exit_request);
  worker_cleanup(client);

}

void handle_payload(unsigned char *buffer)
{
  
}

void parse_frame(WORKER *client)
{
  /* Check if frame queue is full */
  int frame_index = E_ABORT;
  
  for (int i = 0; i < MAX_FRAMES; i++) {
    if (client->frames[i].in_use == false) {
      frame_index = i;
      break;
    }  
  }

  if (frame_index == E_ABORT) {
    LOGF(FATAL, "Maximum websocket frame limit reached");
  }

  /* Check if the frame is a control frame */
  char opcode = client->recv_buffer[0] & 0xF;  /* Lowest 4 bits */

  unsigned char mask_key[4] = {0};
  unsigned __int64 payload_len = client->recv_buffer[1] & 0x7F;
  unsigned char *payload = 0;
  unsigned char payload_index = 0;

  switch (payload_len) {
    case 126: {
      payload_len = 0;
      payload_len |= (unsigned char)client->recv_buffer[2] << 8;
      payload_len |= (unsigned char)client->recv_buffer[3];

      memcpy(&mask_key, &client->recv_buffer[4], 4);
      payload = (unsigned char*)malloc(payload_len+1);
      SecureZeroMemory(payload, payload_len+1);
      payload_index = 8;
      break;
    }
    case 127: {
      
      /* payload_len - most significant bit MUST be set to 0, shift by 55? */
      payload_len = 0;
      payload_len |= (unsigned char)client->recv_buffer[2] << 56;
      payload_len |= (unsigned char)client->recv_buffer[3] << 48;
      payload_len |= (unsigned char)client->recv_buffer[4] << 40;
      payload_len |= (unsigned char)client->recv_buffer[5] << 32;
      payload_len |= (unsigned char)client->recv_buffer[6] << 24;
      payload_len |= (unsigned char)client->recv_buffer[7] << 16;
      payload_len |= (unsigned char)client->recv_buffer[8] << 8;
      payload_len |= (unsigned char)client->recv_buffer[9];
      
      memcpy(&mask_key, &client->recv_buffer[10], 4);
      payload = (unsigned char*)malloc(payload_len+1);
      SecureZeroMemory(payload, payload_len+1);
      payload_index = 14;
      break;
    }
    default:
      memcpy(&mask_key, &client->recv_buffer[2], 4);
      payload = (unsigned char*)malloc(payload_len+1);
      SecureZeroMemory(payload, payload_len+1);
      payload_index = 6;
      break;
  }

  for (int i = 0; i < payload_len; i++, payload_index++) {
    payload[i] = client->recv_buffer[payload_index] ^ mask_key[i % 4];
  }
  

  /* if (opcode == OPCODE_CLOSE) */
  /*   ws_close_handshake(client); */
}


void remove_thread_handle(HANDLE handle, HANDLE *handle_array, HANDLE exit)
{
  /* if exit is requested we should not remove any handles */
  int ret = WaitForSingleObject(exit, NULL);

  if (ret == WAIT_OBJECT_0)
    return;

  EnterCriticalSection(&thread_update.cs);
    
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

  /* Remove thread structure from global thread_list */

  for (int i = 0; i < worker_count; i++) {
    if (thread_list[i]->thread_handle == handle) {
      thread_list[i] = {0};
    }
  }
  
  LeaveCriticalSection(&thread_update.cs);
}

void worker_cleanup(WORKER *client)
{
  EnterCriticalSection(&thread_update.cs);

  int thread_id = client->thread_id;
  
  closesocket(client->socket);
  SetEvent(client->worker_exit_event);
  free(client);
  worker_count--;

  /* Compress array so get_active_sockets can proccess active sockets */
  for (int i = 0, x = worker_count; i < x;) {
    
    /* Valid WORKER object */
    if (thread_list[x] != 0) {
      while (i < x) {
        if (thread_list[i] == 0) {
          thread_list[i] = thread_list[x];
          thread_list[x] = 0;  /* or SecureZeroMemory? */
          x--;
        }
       
        i++;
      }

      /* Redundant */
      if (i >= x)
        break;
    }

    else {
      x--;
      continue;
    }
  }
  
  LOGF(DEBUG, "closing worker thread %d", thread_id);


  LeaveCriticalSection(&thread_update.cs);
}

/* validate_connection? */
int ws_open_handshake(WORKER *client)
{
  static const char request_line[] = "GET / HTTP/1.1\r\n";
  static const char guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

  if (strncmp(request_line, client->recv_buffer, strlen(request_line)) != 0) {
    LOGF(WARNING, "(%d) Invalid HTTP request made", client->thread_id);
    remove_thread_handle(client->thread_handle, client->thread_handles,
                         client->exit_request);
    worker_cleanup(client);
    return E_ABORT;
  }
  
  char *index = strstr(client->recv_buffer, "Sec-WebSocket-Key:");

  if (index == NULL) {
    LOGF(WARNING, "(%d) Invalid HTTP request made", client->thread_id);
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
          "Sec-Websocket-Accept: %S\r\n", encoded_key);
  
  int ret = send(client->socket, response, strlen(response), NULL);

  if (ret == SOCKET_ERROR) {
    LOGF(WARNING, "(%d) send() failed with error %d",
         client->thread_id, WSAGetLastError());
    
    remove_thread_handle(client->thread_handle, client->thread_handles,
                         client->exit_request);
    
    worker_cleanup(client);
    return E_ABORT;
  }

  LOGF(DEBUG, "(%d) established websocket connection", client->thread_id);
  client->in_use = true;
  
  free(encoded_key);
  free(resp_key);

  return EXIT_SUCCESS;
  
}

unsigned __stdcall ws_worker(void *p)
{
  WORKER* client = (WORKER*)p;
  fd_set fdread = {0};
  fd_set fdwrite = {0};
  int ret = 0;
  int recv_ret = 0;

  SecureZeroMemory(&client->recv_buffer, DATA_BUFSIZE);

  while (true) {
    FD_ZERO(&fdread);
    FD_SET(client->socket, &fdread);
    FD_ZERO(&fdwrite);
    FD_SET(client->socket, &fdwrite);
    
    /* Join both select's into one call? */
    if ((recv_ret = select(0, NULL, &fdwrite, NULL, 0)) == SOCKET_ERROR) {
      LOGF(WARNING, "(%d) select() returned with error %d",
           client->thread_id, WSAGetLastError());
      LOGF(WARNING, "(%d) closing worker thread", client->thread_id);
      remove_thread_handle(client->thread_handle, client->thread_handles,
                           client->exit_request);
      worker_cleanup(client);
      return E_ABORT;
    }

    if (recv_ret && client->in_use) {
      if (FD_ISSET(client->socket, &fdwrite)) {
        /* We can write to socket */
      }
    }

    if ((recv_ret = select(0, &fdread, NULL, NULL, &client->interval)) == SOCKET_ERROR) {
      LOGF(WARNING, "(%d) select() returned with error %d",
           client->thread_id, WSAGetLastError());
      LOGF(WARNING, "(%d) closing worker thread", client->thread_id);
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
            send_frame(client, OPCODE_PING);
            
            if (hs == E_ABORT) {
              return E_ABORT;
            }
          }

          /* Websocket state is OPEN */
          else {
            parse_frame(client);
          }
        }
        
        /* Peer has disconnected */
        if (ret == -1 || ret == 0) {
          LOGF(WARNING, "(%d) Peer has disconnected", client->thread_id);
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
  /* get_active_sockets might initialize it before this point */
  if (!thread_update.initialized) {
      InitializeCriticalSection(&thread_update.cs);
  }

  HANDLE exit_event = (HANDLE)p;
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
        /* TODO: Find a cleaner way to wait for a worker thread */
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

      /* TODO: Check for WSAEWOULDBLOCK? */
      if (accept_socket == INVALID_SOCKET) { 
        LOGF(FATAL, "accept() failed with error %d", GetLastError());
        return E_ABORT;
      }

      ret = ioctlsocket(accept_socket, FIONBIO, &non_block);

      if (ret == SOCKET_ERROR) {
        LOGF(FATAL, "ioctlsocket(FIONBIO) failed with error %d", GetLastError());
        return E_ABORT;
      }

      EnterCriticalSection(&thread_update.cs);

      WORKER *thread = (WORKER*)malloc(sizeof(WORKER));
      thread->socket = accept_socket;
      thread->exit_request = exit_event;
      thread->thread_handles = thread_handles;
      thread->in_use = false;
      thread->interval.tv_sec = 0;
      thread->interval.tv_usec = 32000;  /* 32 milliseconds */
      thread->worker_exit_event = worker_exit_event;
      
      SecureZeroMemory(&thread->frames, sizeof(FRAME) * MAX_FRAMES);
      
      thread_handles[worker_count] =
          (HANDLE)_beginthreadex(NULL, NULL, ws_worker,
                                 (void*)thread, NULL,
                                 &thread->thread_id);
      thread_list[worker_count] = thread;
      
      /* TODO: Pause thread so it receives thread handle before starting? */
      thread->thread_handle = thread_handles[worker_count];
      
      SetThreadName("Websocket Worker Thread", thread->thread_id);
      worker_count++;
      LeaveCriticalSection(&thread_update.cs);
      
    }

    else
      FD_SET(listen_socket, &readfds);

    ret = WaitForSingleObject(exit_event, NULL);

    if (ret == WAIT_OBJECT_0) {
      WaitForMultipleObjects(worker_count, thread_handles, true, INFINITE);
      LOGF(DEBUG, "All workers exited. Closing Websocket thread manager");
      closesocket(listen_socket);
      WSACleanup();
      DeleteCriticalSection(&thread_update.cs);
      return EXIT_SUCCESS;
    }
  
  }  
}
