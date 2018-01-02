# Builds external third party projects and finds libraries
# The parent script needs to define the "GLOBAL_OUTPUT_PATH" variable,
# which will be used as output directory for all *.lib, *.dll, *.a, *.so, *.pdb files

include_directories(${extern_dir})

set(EXTERNAL_IDE_FOLDER "external")

# include(${extern_dir}/imgui.cmake)
include(${extern_dir}/gflags.cmake)
include(${extern_dir}/irsdk.cmake)
include(${extern_dir}/json.cmake)
include(${extern_dir}/g3logger.cmake)
include(${extern_dir}/libmicrohttpd.cmake)
include(${extern_dir}/websocketpp.cmake)
include(${extern_dir}/openssl.cmake)
include(${extern_dir}/asio.cmake)

# Qt5

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

if(Qt5_FOUND AND WIN32 AND TARGET Qt5::qmake AND NOT TARGET Qt5::windeployqt)
  get_target_property(_qt5_qmake_location Qt5::qmake IMPORTED_LOCATION)

  execute_process(
    COMMAND "${_qt5_qmake_location}" -query QT_INSTALL_PREFIX
    RESULT_VARIABLE return_code
    OUTPUT_VARIABLE qt5_install_prefix
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

  set(imported_location "${qt5_install_prefix}/bin/windeployqt.exe")

  if(EXISTS ${imported_location})
    add_executable(Qt5::windeployqt IMPORTED)

    set_target_properties(Qt5::windeployqt PROPERTIES
      IMPORTED_LOCATION ${imported_location}
      )
  endif()
endif()
