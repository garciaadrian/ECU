#
# Configuration for all sub-projects
#

# Generate version-header
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/${META_PROJECT_NAME}-version.h)

# Add src/ directory to include paths to conform to Google header styleguide
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Sub-projects
#

# Categorize sub-projects
set(IDE_FOLDER "src")

add_subdirectory(base)
add_subdirectory(libir)
add_subdirectory(atlas)
add_subdirectory(car)
add_subdirectory(hid)
add_subdirectory(hid/g27)
add_subdirectory(vjoy-feeder)
add_subdirectory(ui)


# Main executable
add_subdirectory(main)

# Tests


#
# Deployment
#

# Deploy generated headers
# install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/ DESTINATION include  COMPONENT dev)
