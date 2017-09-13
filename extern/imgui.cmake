set(target imgui)

ExternalProject_Add(
  IMGUI

  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG "master"

  BUILD_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""

  SOURCE_DIR "${CMAKE_SOURCE_DIR}/extern/imgui"
  CMAKE_ARGS

  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  TEST_COMMAND ""
  )

set(IMGUI_SOURCE_DIR ${extern_dir}/imgui)

set(IMGUI_SOURCES
  ${IMGUI_SOURCE_DIR}/imgui.cpp
  ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
  )

set_target_properties(${target}
  PROPERTIES
  ${DEFAULT_PROJECT_OPTIONS}
  FOLDER "${EXTERNAL_IDE_FOLDER}"
  )

add_library(${target} ${IMGUI_SOURCES})
