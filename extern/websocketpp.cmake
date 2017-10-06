set(target websocketpp)

set(WEBSOCKETPP_SOURCE_DIR ${extern_dir}/websocketpp)

set(WEBSOCKETPP_SOURCES
  )

add_library(${target} INTERFACE)

# target_sources(${target} INTERFACE ${WEBSOCKETPP_SOURCES})

target_include_directories(${target}
  INTERFACE
  ${WEBSOCKETPP_SOURCE_DIR}
  )
