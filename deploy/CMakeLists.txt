
# 
# Target 'pack'
# 

add_custom_target(pack)
set_target_properties(pack
  PROPERTIES
  EXCLUDE_FROM_DEFAULT_BUILD 1
  FOLDER "Maintenance"
  )


# Install additional runtime dependencies
include(${PROJECT_SOURCE_DIR}/cmake/RuntimeDependencies.cmake)


# 
# Packages
# 

include(packages/pack-ecu.cmake)


#
# Target 'component_install'
#

add_custom_target(
    component_install
    COMMAND make preinstall
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/ComponentInstall.cmake
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

set_target_properties(
  component_install
  PROPERTIES
  FOLDER "Maintenance"
  EXCLUDE_FROM_DEFAULT_BUILD 1
  )
