set(target libmicrohttpd)

if("${CMAKE_BUILD_TYPE}" MATCHES "")
  set(LIB_FILE ${extern_dir}/libmicrohttpd/x86_64/VS2015/Debug-static/libmicrohttpd_d.lib)
  add_library(${target} STATIC IMPORTED ${LIB_FILE})
  
else()
  set(LIB_FILE ${extern_dir}/libmicrohttpd/x86_64/VS2015/Release-static/libmicrohttpd.lib)
  add_library(${target} STATIC IMPORTED ${LIB_FILE})
  
endif()

set_target_properties(${target}
  PROPERTIES
  IMPORTED_LOCATION ${LIB_FILE}
  INTERFACE_INCLUDE_DIRECTORIES ${extern_dir}/libmicrohttpd/x86_64/VS2015/Release-static/
  )
