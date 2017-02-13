
group("src")
project("ECU-server")
  uuid("fb411540-d629-4a07-97f6-744601bb97f8")
  kind("StaticLib")
  language("C++")
  files({
    "http.cpp",
    "http.h",
    "client.h",
    "websocket.cpp",
    "websocket.h",
  })
  links({
  })
