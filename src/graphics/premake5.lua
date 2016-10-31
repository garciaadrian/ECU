
group("src")
project("ECU-graphics")
  uuid("27ad9235-df81-4095-a555-aa01e2ca094f")
  kind("StaticLib")
  language("C++")
  files({
    "app.cpp",
    "app.h",
    "app_handler.cpp",
    "app_handler.h",
    "app_handler_win.cpp",
  })
  links({
    "libcef",
    "irsdk",
  })
