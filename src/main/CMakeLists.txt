
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
  main_resources.rc
  )


#
# Create executable
#

# Build executable
add_executable(${target}
  WIN32
  ${sources}
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
  ${DEFAULT_LIBRARIES}
  ${META_PROJECT_NAME}::baselib
  ${META_PROJECT_NAME}::libir
  ${META_PROJECT_NAME}::hid
  ${META_PROJECT_NAME}::hid-g27 
  ${META_PROJECT_NAME}::ui
  ${META_PROJECT_NAME}::car
  gflags
  irsdk
  json
  g3logger
  )


#
# Compile definitions
#

target_compile_definitions(${target}
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