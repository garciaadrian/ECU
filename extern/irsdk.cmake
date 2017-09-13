set(target irsdk)

set(IRSDK_SOURCE_DIR ${extern_dir}/irsdk)

set(IRSDK_SOURCES
  ${IRSDK_SOURCE_DIR}/irsdk_client.cpp
  ${IRSDK_SOURCE_DIR}/irsdk_client.h
  ${IRSDK_SOURCE_DIR}/irsdk_diskclient.cpp
  ${IRSDK_SOURCE_DIR}/irsdk_diskclient.h
  ${IRSDK_SOURCE_DIR}/irsdk_defines.h
  ${IRSDK_SOURCE_DIR}/irsdk_utils.cpp
  ${IRSDK_SOURCE_DIR}/yaml_parser.cpp
  ${IRSDK_SOURCE_DIR}/yaml_parser.h
  )

add_library(irsdk ${IRSDK_SOURCES})

set_target_properties(${target}
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${EXTERNAL_IDE_FOLDER}"
  )

target_include_directories(${target}
  PUBLIC
  ${IRSDK_SOURCE_DIR}
  )
