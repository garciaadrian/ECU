group("src")
project("ECU-vjoy")
  uuid("b5922b8e-d9db-4a70-b93b-f677be11a9ea")
  kind("StaticLib")
  language("C++")
  files({
    "feeder.cc",
    "feeder.h",
    "joy_consumer.h",
    "joy_consumer.cc",
  })