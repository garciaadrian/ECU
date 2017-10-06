set(target openssl)

# TODO(garciaadrian): Link openssl libs based on runtime library

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
  set(OPENSSL_LIB ${extern_dir}/openssl/openssl-1.1.0e-vs2017/lib64/libsslMTd.lib)
  set(OPENSSL_CRYPT_LIB ${extern_dir}/openssl/openssl-1.1.0e-vs2017/lib64/libcryptoMTd.lib)
  
  add_library(openssl STATIC IMPORTED ${OPENSSL_LIB})
  add_library(openssl-crypt STATIC IMPORTED ${OPENSSL_CRYPT_LIB})
  
else()
  set(OPENSSL_LIB ${extern_dir}/openssl/openssl-1.1.0e-vs2017/lib64/libsslMT.lib)
  set(OPENSSL_CRYPT_LIB ${extern_dir}/openssl/openssl-1.1.0e-vs2017/lib64/libcryptoMT.lib)

  add_library(openssl STATIC IMPORTED ${OPENSSL_LIB})
  add_library(openssl-crypt STATIC IMPORTED ${OPENSSL_CRYPT_LIB})
  
endif()

set_target_properties(openssl
  PROPERTIES
  IMPORTED_LOCATION ${OPENSSL_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${extern_dir}/openssl/openssl-1.1.0e-vs2017/include64/
  )

set_target_properties(openssl-crypt
  PROPERTIES
  IMPORTED_LOCATION ${OPENSSL_CRYPT_LIB}
  INTERFACE_INCLUDE_DIRECTORIES ${extern_dir}/openssl/openssl-1.1.0e-vs2017/include64/
  )
