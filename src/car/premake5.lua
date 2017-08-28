group("src")
project("ECU-car")
  uuid("45a8a31c-019c-42d5-8599-f153bed12257")
  kind("StaticLib")
  language("C++")
  links({
    "ECU-libir",
    "ECU-vjoy"
  })
  files({
    "ecu.cc",
    "ecu.h",
    "iracing_input.h",
    "iracing_input.cc",
    "brakes/brakes.h",
    "brakes/brakes.cc",
    "brakes/input_commands.h",
    "brakes/console_commands.h",
    })
    


