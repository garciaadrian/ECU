group("src")
project("ECU-libir")
  uuid("b09c8ac6-0ca1-4104-9a63-20a284852998")
  kind("StaticLib")
  language("C++")
  links({
    "irsdk"
  })
  files({
    "telemetry.h",
    "telemetry.cc",
    "iracing.h",
    "iracing.cc",
    "listener.h",
    "events.h",
    "events.cc",
    "delegate.h",
    "tick.h",
    "tick.cc",
    })
    


