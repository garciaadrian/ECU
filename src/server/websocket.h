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

#ifndef SERVER_WEBSOCKET_H_
#define SERVER_WEBSOCKET_H_

#define ASIO_STANDALONE

// TODO(garciaadrian): websocketpp can't detect we're in c++11 so we define this
#define _WEBSOCKETPP_CPP11_STL_

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <functional>

typedef websocketpp::server<websocketpp::config::asio> server;

namespace ecu {
namespace websocket {

class WebsocketServer {
 public:
  WebsocketServer();
  ~WebsocketServer();

  void Run();

 private:
  server endpoint_;
};

}  // namespace websocket
}  // namespace ecu

#endif // SERVER_WEBSOCKET_H_
