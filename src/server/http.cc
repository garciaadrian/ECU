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

#include "server/http.h"

#include <fstream>

#include "g3log/g3log.hpp"

namespace ecu {
namespace server {

int AnswerConnection(void* cls, struct MHD_Connection* connection,
                     const char* url, const char* method,
                     const char* version, const char* upload_data,
                     size_t* upload_data_size, void** con_cls) {
  if (!::strcmp(url, "/")) {
    
    std::ifstream file("static/index.html", std::ios::binary);
    
    if (!file.is_open()) {
      return 404;
    }

    std::string contents((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    auto contents_raw = contents.c_str();

    struct MHD_Response* response =
        MHD_create_response_from_buffer(contents.length(),
                                        &contents_raw,
                                        MHD_RESPMEM_MUST_COPY);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);

    return ret;
  }

  std::string final_url = "static/";
  final_url += url;

  std::ifstream file(final_url, std::ios::binary);

  if (!file.is_open()) {
    return 404;
  }

  std::string contents((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
  auto contents_raw = contents.c_str();

  struct MHD_Response* response =
      MHD_create_response_from_buffer(contents.length(),
                                      &contents_raw,
                                      MHD_RESPMEM_MUST_COPY);
  int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
  MHD_destroy_response(response);
  
  return ret;
}


HttpServer::HttpServer(const std::string& directory, int port)
    : directory_(directory), port_(port) {}
HttpServer::~HttpServer() {}

void HttpServer::Serve() {
  daemon_ = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, port_, nullptr, nullptr,
                             &AnswerConnection, nullptr, MHD_OPTION_END);
  if (daemon_ == nullptr) {
    LOGF(g3::FATAL, "Failed to start HTTP server");
  }
}


void HttpServer::Quit() {

}

}  // namespace ecu
}  // namespace server
