set(target gflags)

add_subdirectory(${extern_dir}/gflags)

set_target_properties(gflags_nothreads_static
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${EXTERNAL_IDE_FOLDER}"
  VERSION ${META_VERSION}
  SOVERSION ${META_VERSION_MAJOR}
  )

