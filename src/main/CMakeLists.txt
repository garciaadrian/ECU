
#
# External dependencies
#

# find_package(THIRDPARTY REQUIRED)


#
# Executable name and options
#

# Target name
set(target ECU)

# Exit here if required dependencies are not met
message(STATUS "Executable ${target}")


#
# Sources
#

set(sources
  ecu-core.cc
  resource.h
  )


#
# Create executable
#

# Build executable
add_executable(${target}
  WIN32
  ${sources}
  main_resources.rc
  )

if(TARGET Qt5::windeployqt)
  # execute windeployqt in a tmp directory then copy to bin directory
  add_custom_command(TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
    COMMAND set PATH=%PATH%$<SEMICOLON>${qt5_install_prefix}/bin
    COMMAND Qt5::windeployqt --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
    "$<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_NAME:${target}>"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" "$<TARGET_FILE_DIR:${target}>"
    )

  # copy deployment directory during installation
  install(
    DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/"
    DESTINATION ${INSTALL_DATA}
    COMPONENT runtime
    )
endif()

set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE)

include(InstallRequiredSystemLibraries)

install(
    PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
    DESTINATION ${INSTALL_BIN}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})

#
# Project options
#
set_target_properties(${target}
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${IDE_FOLDER}"

  VS_DEBUGGER_WORKING_DIRECTORY
  $(TargetPath)\..

  LINK_FLAGS_RELEASE
  /DEBUG                        # Create PDB
  /OPT:REF                      # Prevent assembly copies
  /OPT:ICF
  )

#
# Include directories
#

target_include_directories(${target}
  PRIVATE
  ${DEFAULT_INCLUDE_DIRECTORIES}
  )

#
# Libraries
#

target_link_libraries(${target}
  PRIVATE
  Qt5::Widgets
  ${DEFAULT_LIBRARIES}
  ${META_PROJECT_NAME}::ui
  gflags
  irsdk
  json
  g3logger

  PUBLIC
  ${DEFAULT_LINKER_OPTIONS}
  )


#
# Compile definitions
#

target_compile_definitions(${target}
  PRIVATE
  ${DEFAULT_COMPILE_DEFINITIONS}
  )


#
# Compile options
#

target_compile_options(${target}
  PRIVATE
  ${DEFAULT_COMPILE_OPTIONS}
  )

#
# Linker options
#
target_link_libraries(${target}
  PRIVATE
  ${DEFAULT_LINKER_OPTIONS}
  )

#
# Target Health
#

# perform_health_checks(
#   ${target}
#   ${sources}
#   )


#
# Deployment
#

# Executable
install(TARGETS ${target}
  RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT runtime
  BUNDLE DESTINATION ${INSTALL_BIN} COMPONENT runtime
  )
