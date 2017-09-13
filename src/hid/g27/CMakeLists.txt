
#
# External dependencies
#

# find_package(g3logger REQUIRED)

#
# Library name and options
#

# Target name
set(target hid-g27)

# Exit here if required dependencies are not met
message(STATUS "Lib ${target}")

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(feature_file         "include/${target}/${target}_features.h")
set(export_file          "include/${target}/${target}_export.h")
set(template_export_file "include/${target}/${target}_api.h")
set(export_macro         "${target_id}_API")


#
# Sources
#

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}")
set(source_path "${CMAKE_CURRENT_SOURCE_DIR}")

set(headers
  )

set(sources
  ${source_path}/g27_hid.cc
  ${source_path}/g27_hid.h
  ${source_path}/g27_input_driver.cc
  ${source_path}/g27_input_driver.h
  )

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$"
  ${header_group} ${headers})
source_group_by_path(${source_path} "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
  ${source_group} ${sources})


#
# Create library
#

# Build library
add_library(${target}
  ${sources}
  ${headers}
  )

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Create API export header
generate_export_header(${target}
  EXPORT_FILE_NAME ${export_file}
  EXPORT_MACRO_NAME ${export_macro}
  )

generate_template_export_header(${target}
  ${target_id}
  ${template_export_file}
  )

#
# Project options
#

set_target_properties(${target}
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${IDE_FOLDER}"
  VERSION ${META_VERSION}
  SOVERSION ${META_VERSION_MAJOR}
  )

#
# Include directories
#

# target_include_directories(${target}


#
# Libraries
#

target_link_libraries(${target}
  PRIVATE
  ${DEFAULT_LIBRARIES}
  ${META_PROJECT_NAME}::hid
  Setupapi
  Hid
  Winusb
  )

#
# Compile Definitions
#

target_compile_definitions(${target}
  PRIVATE

  PUBLIC
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${target_id}_STATIC_DEFINE>
  ${DEFAULT_COMPILE_DEFINITIONS}

  INTERFACE
  )

#
# Target Health
#

# perform_health_checks(
#   ${target}
#   ${sources}
#   ${headers}
#   )
  
