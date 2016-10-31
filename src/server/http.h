#ifndef HTTP_H
#define HTTP_H

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <sys/types.h>

typedef SSIZE_T ssize_t; // Windows type to unix type
typedef UINT64 uint64_t; // Windows type to unix type
typedef UINT16 uint16_t; // Windows type to unix type
#include "microhttpd.h"

#define PORT 9008

int answer_to_connection(void* cls, struct MHD_Connection* connection,
                         const char* url, const char* method,
                         const char* version, const char* upload_data,
                         size_t* upload_data_size, void** con_cls);

#endif
