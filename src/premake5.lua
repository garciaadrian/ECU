group("src")
project("ECU-core")
uuid("02054839-d1cf-4dbf-bc7b-5cc12097ed82")
kind("WindowedApp")
targetname("ECU")
language("C++")

files({
  "ecu-core.cc",
  "ecu-core.h",
})

files({
  "resource.h",    
  "main_resources.rc",
})

links({
  "inih",
  "ECU-hid",
  "ECU-hid-g27",
  "ECU-ui",
  "ECU-base",
  "Hid",
  "Crypt32",
  "Setupapi",
  "Winusb",
})

filter("configurations:Debug")
  linkoptions({
    "/ENTRY:WinMainCRTStartup",
})

filter("configurations:Release")
  linkoptions({
    "/RELEASE",
  })
flags({
  "WinMain",
})
