set(target json)

set(JSON_SOURCE_DIR ${extern_dir}/json)

set(JSON_SOURCES
  ${JSON_SOURCE_DIR}/json.hpp
  )

add_library(${target} INTERFACE)

# target_sources(${target} INTERFACE ${JSON_SOURCES})

target_include_directories(${target}
  INTERFACE
  ${JSON_SOURCE_DIR}
  )
