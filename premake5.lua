include("tools/build/export-compile-commands/export-compile-commands")


location("build")
targetdir("build/bin")
objdir("build/obj")

includedirs({
 ".",
 "src",
 "libs/inih",
 "libs/json",
 "libs/gflags/src",
 "libs/cereal/include",
})

defines({ "_UNICODE", "UNICODE", "STRICT", "NOMINMAX", })

filter("platforms:Windows")
  system("windows")
  toolset("msc")
  buildoptions({
    "/MP",
    "/FC",
  })
  flags({
    "NoMinimalRebuild",
    "Symbols",
    "StaticRuntime",
  })
  defines({
    "_WIN64=1",
    "WIN32_LEAN_AND_MEAN",
    "OS_WIN",
    "D_SCL_SECURE_NO_WARNINGS",
    "_SCL_SECURE_NO_WARNINGS",

  })

filter("configurations:Release")
  targetdir("build/bin/Release")

  includedirs({
        "libs/g3logger/src",
        "libs/libmicrohttpd/x86_64/VS2015/Release-static/",
        "libs/sqlite/",
        "libs/json",
        "libs",
  })
  libdirs({
    "libs/g3logger/build/Release",
    "libs/libmicrohttpd/x86_64/VS2015/Release-static/",
    "libs/vJoydriver/lib/amd64/",
  })
  links({
    "gflags",
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "Winmm",
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "libmicrohttpd",
    "Bcrypt",
    "g3logger",
    "vJoyInterface",
  })
  linkoptions({
    "/ignore:4099",
  })
  flags({
    "Optimize",
  })
  runtime("Release")
  defines({
    "NDEBUG",
    "_NDEBUG",
    "USING_CEF_SHARED",
    "CEF_USE_SANDBOX",
  })

filter("configurations:Debug")
  targetdir("build/bin/Debug")  
  includedirs({
        "libs/g3logger/src",
        "libs/libmicrohttpd/x86_64/VS2015/Debug-static/",
        "libs/sqlite/",
        "libs/json",
        "libs",
   })
  libdirs({
    "libs/g3logger/build/Debug",
    "libs/libmicrohttpd/x86_64/VS2015/Debug-static/",
    "libs/vJoydriver/lib/amd64/",
    })
  links({
    "gflags",
    "Winmm",
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "libmicrohttpd_d",
    "Bcrypt",
    "g3logger",
    "vJoyInterface",
  })
  linkoptions({
    "/ignore:4099",
  })
  runtime("Debug")
  defines({
    "_DEBUG",
    "_NO_DEBUG_HEAP=1",
    "USING_CEF_SHARED",
    "CEF_USE_SANDBOX",
    "ws2_32",
    "_CRT_SECURE_NO_WARNINGS",
  })

solution("ECU")
  uuid("9d6fa407-66c8-4f1f-942f-823e036ccd49")
  architecture("x86_64")
  if os.is("windows") then
    platforms({"Windows"})
  end
  configurations({"Debug", "Release"})

  -- Include third party libs first
  include("libs/gflags.lua")
  include("libs/vJoydriver.lua")
  include("libs/inih")
  include("libs/sqlite3")
  include("libs/irsdk")
  
  include("src/")
  include("src/base")
  include("src/vjoy-feeder")
  include("src/libir")
  include("src/hid")
  include("src/hid/g27")
  include("src/ui")
  include("src/car")