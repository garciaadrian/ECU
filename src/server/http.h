/**
*******************************************************************************
*                                                                             *
* ECU: iRacing Extensions Collection Project                                  *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#ifndef SERVER_HTTP_H_
#define SERVER_HTTP_H_

#include <WS2tcpip.h>
#include <Winsock2.h>
#include <sys/types.h>
#include <string>

#include "microhttpd.h"

namespace ecu {
namespace server {

typedef SSIZE_T ssize_t;
typedef UINT64 uint64_t;
typedef UINT16 uint16_t;

class HttpServer {
 public:
  HttpServer(const std::string& directory, int port);
  ~HttpServer();

  void Serve();
  void Quit();

 private:
  struct MHD_Daemon* daemon_;
  std::string directory_;
  int port_ = 0;
};

}  // namespace server
}  // namespace ecu

#endif // SERVER_HTTP_H_
