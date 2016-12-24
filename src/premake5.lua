group("src")
project("ECU-core")
uuid("02054839-d1cf-4dbf-bc7b-5cc12097ed82")
kind("WindowedApp")
targetname("ECU")
language("C++")

files({
  "ecu-core.cpp",
  "ecu-core.h",
})

files({
  "main_resources.rc",
})

links({
  "sqlite",
  "irsdk",
  "inih",
  "libcef",
  "libcef_dll_wrapper",
  "dxguid",
  "dinput8",
  "ECU-base",
})
filter("configurations:Release")
  linkoptions({
    "/RELEASE",
  })
flags({
  "WinMain",
})
