group("libs")
project("gflags")
  uuid("e319da87-75ed-4517-8f65-bd25e9cc02a3")
  kind("StaticLib")
  language("C++")

  -- These win32-specific overrides must come before others.
  filter("platforms:Windows")
    defines({
      "PATH_SEPARATOR=%%27\\\\%%27",
    })
    includedirs({
      "gflags/src/windows",
    })
  filter({})
  
  includedirs({
    "gflags/src",
  })
  
  defines({
    "GFLAGS_DLL_DECL=",
    "GFLAGS_DLL_DEFINE_FLAG=",
    "GFLAGS_DLL_DECLARE_FLAG=",
    "_LIB",
  })
  
  files({
    "gflags/src/gflags.cc",
    "gflags/src/gflags_completions.cc",
    "gflags/src/gflags_reporting.cc",
    "gflags/src/mutex.h",
    "gflags/src/util.h",
  })
  filter("platforms:Windows")
    files({
      "gflags/src/windows/config.h",
      "gflags/src/windows/gflags/gflags.h",
      "gflags/src/windows/gflags/gflags_completions.h",
      "gflags/src/windows/gflags/gflags_declare.h",
      "gflags/src/windows/port.cc",
      "gflags/src/windows/port.h",
    })