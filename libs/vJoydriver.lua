group("libs")
project("vJoydriver")
  uuid("fb44cf4a-d8c7-4263-9d87-d8a3393aa9ae")
  kind("StaticLib")
  language("C++")
  
  includedirs({
    "vJoydriver",
  })

  libdirs({
    "vJoydriver/lib/amd64/",  
  })

  links({
    "vJoyInterface",
  })