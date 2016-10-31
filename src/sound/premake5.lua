
group("src")
project("ECU-sound")
  uuid("abd914b5-c336-4224-a798-e5bacb576bb4")
  kind("StaticLib")
  language("C++")
  files({
    "xinput2.cpp",
    "xinput2.h",
  })
  links({
    "irsdk",
  })
