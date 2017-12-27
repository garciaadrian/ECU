#
# Configuration for all sub-projects
#

# Generate version-header
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/${META_PROJECT_NAME}-version.h)

# Add src/ directory to include paths to conform to Google header styleguide
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Enable compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Sub-projects
#

# Categorize sub-projects
set(IDE_FOLDER "src")

add_subdirectory(ui)


# Main executable
add_subdirectory(main)

# Tests
if(OPTION_BUILD_TESTS)
    set(IDE_FOLDER "tests")
    add_subdirectory(tests)
endif()

#
# Deployment
#

# Deploy generated headers
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/ DESTINATION include  COMPONENT dev)
