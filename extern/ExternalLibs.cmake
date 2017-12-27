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

# CMAKE

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
