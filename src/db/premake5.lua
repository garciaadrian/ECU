group("src")
project("ECU-db")
uuid("27c5a55c-8e81-4fb4-b7fa-ba1fc2d5e3ce")
kind("StaticLib")
language("C++")

files({
  "sqlite.h",
  "sqlite.cpp",
})
links({
      "sqlite",
})
