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

#include "server/websocket.h"

namespace ecu {
namespace websocket {

WebsocketServer::WebsocketServer() {
  endpoint_.set_error_channels(websocketpp::log::elevel::all);
  endpoint_.set_access_channels(websocketpp::log::alevel::all ^
                                websocketpp::log::alevel::frame_payload);
  endpoint_.init_asio();
}

WebsocketServer::~WebsocketServer() {}

void WebsocketServer::Run() {
  endpoint_.listen(9002);

  endpoint_.start_accept();

  endpoint_.run();
}

}  // namespace websocket
}  // namespace ecu
