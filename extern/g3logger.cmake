set(target g3logger)

set(G3LOGGER_SOURCE_DIR ${extern_dir}/g3logger/src)

set(G3LOGGER_SOURCES
  ${G3LOGGER_SOURCE_DIR}/crashhandler_windows.cpp
  ${G3LOGGER_SOURCE_DIR}/filesink.cpp
  ${G3LOGGER_SOURCE_DIR}/g3log.cpp
  ${G3LOGGER_SOURCE_DIR}/logcapture.cpp
  ${G3LOGGER_SOURCE_DIR}/loglevels.cpp
  ${G3LOGGER_SOURCE_DIR}/logmessage.cpp
  ${G3LOGGER_SOURCE_DIR}/logworker.cpp
  ${G3LOGGER_SOURCE_DIR}/stacktrace_windows.cpp
  ${G3LOGGER_SOURCE_DIR}/time.cpp
  )

add_library(${target} ${G3LOGGER_SOURCES})

target_include_directories(${target}
  PUBLIC
  ${G3LOGGER_SOURCE_DIR}
  )

set_target_properties(${target}
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${EXTERNAL_IDE_FOLDER}"
  )
