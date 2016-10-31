group("src")
project("ECU-base")
  uuid("13eba91a-94d1-4533-a5ba-af5a3019df52")
  kind("StaticLib")
  language("C++")
  files({
        "base.h",
        "startup.h",
        "startup.cpp",
        "loop.h",
        "loop.cpp",
        "irsdk_vars.h",
        "app.h",
        "app.cpp",
        "debug.h",
        "debug.cpp",
        "track.cpp",
        "track.h",
        "ir_helper.h",
        "ir_helper.cpp",
        "websocket.cpp",
        "websocket.h",
  })
  links({
        "irsdk",
        "sqlite",
        "ECU-graphics",
        "ECU-sound",
        "ECU-car",
        "ECU-hid",
        "ECU-server",
        "ECU-db",
  })
