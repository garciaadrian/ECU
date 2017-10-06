set(target asio)

set(ASIO_SOURCE_DIR ${extern_dir}/asio)

set(ASIO_SOURCES
  )

add_library(${target} INTERFACE)

# target_sources(${target} INTERFACE ${JSON_SOURCES})

target_include_directories(${target}
  INTERFACE
  ${ASIO_SOURCE_DIR}/asio/include/
  )
