#
# External dependencies
#

# find_package(g3logger REQUIRED)

#
# Library name and options
#

# Target name
set(target ffb)

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
  ${source_path}/feeder.h
  ${source_path}/feeder.cc
  ${source_path}/di_device.h
  ${source_path}/di_device.cc
  ${source_path}/ffb_thread.h
  ${source_path}/ffb_thread.cc
  ${source_path}/ffb_monitor.cc
  ${source_path}/ffb_monitor.h
  ${source_path}/di_manager.h
  ${source_path}/di_manager.cc
  ${source_path}/atomicops.h
  ${source_path}/readerwriterqueue.h
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

# TODO(garciaadrian): Move this to external dependencies
set(VJOY_INTERFACE_DLL "${extern_dir}/vJoydriver/lib/amd64/vJoyInterface.dll")
set(VJOY_INTERFACE_LIB "${extern_dir}/vJoydriver/lib/amd64/vJoyInterface.lib")

add_custom_command(TARGET ${target} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${VJOY_INTERFACE_DLL}"
  $<TARGET_FILE_DIR:${target}>
  )

install(FILES ${VJOY_INTERFACE_DLL}
  DESTINATION ${INSTALL_BIN} COMPONENT runtime
  )

target_link_libraries(${target}
  PRIVATE
  Qt5::Widgets
  dxguid
  dinput8
  Winmm
  ${VJOY_INTERFACE_LIB}
  ${DEFAULT_LIBRARIES}

  PUBLIC
  ${DEFAULT_LINKER_OPTIONS}
  )

#
# Compile Definitions
#

target_compile_definitions(${target}
  PRIVATE
  ${DEFAULT_COMPILE_DEFINITIONS}
  
  PUBLIC
  $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${target_id}_STATIC_DEFINE>


  INTERFACE
  )

#
# Compile Options
#

target_compile_options(${target}
  PRIVATE
  ${DEFAULT_COMPILE_OPTIONS}
  )

#
# Target Health
#

# perform_health_checks(
#   ${target}
#   ${sources}
#   ${headers}
#   )
