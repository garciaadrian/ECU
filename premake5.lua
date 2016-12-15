location("build")
targetdir("build/bin")
objdir("build/obj")

includedirs({
 ".",
 "src",
 "libs/cef_binary_3.2704.1414.g185cd6c_windows64/",
 "libs/inih",
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
        "libs/libmicrohttpd/x86_64/VS2015/Release-static/",
        "libs/sqlite/",
        "libs",
  })
  libdirs({
    "libs/cef_binary_3.2704.1414.g185cd6c_windows64/Release",
    "libs/cef_binary_3.2704.1414.g185cd6c_windows64/build/libcef_dll_wrapper/Release/",
    "libs/libmicrohttpd/x86_64/VS2015/Release-static/"
  })
  links({
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "libcef_dll_wrapper",
    "libcef",
    "cef_sandbox",
    "Winmm",
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "cef_sandbox",
    "libmicrohttpd",
    "Bcrypt"
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
        "libs/libmicrohttpd/x86_64/VS2015/Debug-static/",
        "libs/sqlite/",
        "libs",
   })
  libdirs({
    "libs/cef_binary_3.2704.1414.g185cd6c_windows64/Debug",
    "libs/cef_binary_3.2704.1414.g185cd6c_windows64/build/libcef_dll_wrapper/Debug/",
    "libs/libmicrohttpd/x86_64/VS2015/Debug-static/",
    })
  links({
    "Winmm",
    "comctl32",
    "rpcrt4",
    "shlwapi",
    "ws2_32",
    "version",
    "Dbghelp",
    "kernel32",
    "psapi",
    "cef_sandbox",
    "libmicrohttpd_d",
    "Bcrypt",
  })
  linkoptions({
    "/ignore:4099",
  })
  runtime("Debug")
  defines({
    "DEBUG",
    "_DEBUG",
    "_NO_DEBUG_HEAP=1",
    "USING_CEF_SHARED",
    "CEF_USE_SANDBOX",
    "ws2_32",
  })

solution("ECU")
  uuid("9d6fa407-66c8-4f1f-942f-823e036ccd49")
  architecture("x86_64")
  if os.is("windows") then
    platforms({"Windows"})
  end
  configurations({"Debug", "Release"})

  include("libs/inih")
  include("libs/sqlite3")
  include("libs/irsdk")
  include("src/")
  include("src/sound")
  include("src/hid")
  include("src/base")
  include("src/db")
  include("src/graphics")
  include("src/server")
  include("src/car")


