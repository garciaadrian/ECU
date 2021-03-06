
# 
# Platform and architecture setup
# 

# Get upper case system name
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

# Determine architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(X64 ON)
endif()


# 
# Project options
#

set(LINK_FLAGS_RELEASE
  /LTCG /LTCG:incremental
  )

set(LINK_FLAGS_DEBUG
  /Debug /OPT:REF /OPT:ICF
  )

set(DEFAULT_PROJECT_OPTIONS
    DEBUG_POSTFIX             "d"
    CXX_STANDARD              14 # Not available before CMake 3.1; see below for manual command line argument addition
    LINKER_LANGUAGE           "CXX"
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET     "hidden"    
 )


# 
# Include directories
# 

set(DEFAULT_INCLUDE_DIRECTORIES)


# 
# Libraries
# 

set(DEFAULT_LIBRARIES)


#
# Linker flags
#

set(DEFAULT_LINKER_OPTIONS)

# 
# Compile definitions
# 

set(DEFAULT_COMPILE_DEFINITIONS
    SYSTEM_${SYSTEM_NAME_UPPER}
    )

# MSVC/clang-cl compiler definitions
if ("${SYSTEM_NAME_UPPER}" MATCHES "WINDOWS")
    set(DEFAULT_COMPILE_DEFINITIONS ${DEFAULT_COMPILE_DEFINITIONS}
        _SCL_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the Standard C++ Library
        _CRT_SECURE_NO_WARNINGS  # Calling any one of the potentially unsafe methods in the CRT Library
        _UNICODE
        UNICODE
        NOMINMAX                 # Silence warning C4003
        WIN32_LEAN_AND_MEAN      # Prevent Windows.h from including Winsock.
    )
endif ()


# 
# Compile options
# 

set(DEFAULT_COMPILE_OPTIONS)

# MSVC/clang-cl compiler and linker options
if ("${SYSTEM_NAME_UPPER}" MATCHES "WINDOWS")
  set(CMAKE_CXX_WARNING_LEVEL 4)
  IF (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    STRING(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  ELSE (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  ENDIF (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
  
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        /MP           # -> build with multiple processes
        /W4           # -> warning level 4
        # /WX         # -> treat warnings as errors

        /wd4251       # -> disable warning: 'identifier': class 'type' needs to have dll-interface to be used by clients of class 'type2'
        /wd4592       # -> disable warning: 'identifier': symbol will be dynamically initialized (implementation limitation)
        # /wd4201     # -> disable warning: nonstandard extension used: nameless struct/union (caused by GLM)
        # /wd4127     # -> disable warning: conditional expression is constant (caused by Qt)
        
        #$<$<CONFIG:Debug>:
        #/RTCc         # -> value is assigned to a smaller data type and results in a data loss
        #>

        $<$<CONFIG:Release>: 
        /Gw           # -> whole program global optimization
        /GS-          # -> buffer security check: no 
        /GL           # -> whole program optimization: enable link-time code generation (disables Zi)
        /GF           # -> enable string pooling
        /Ox
        /Zc:inline
        >
        
        # No manual c++11 enable for MSVC as all supported MSVC versions for cmake-init have C++11 implicitly enabled (MSVC >=2013)
        )
endif ()

# GCC and Clang ON LINUX compiler options
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}"
    MATCHES "Clang" AND NOT "${SYSTEM_NAME_UPPER}" MATCHES "WINDOWS")
    set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
        -Wall
        -Wextra
        -Wunused

        -Wreorder
        -Wignored-qualifiers
        -Wmissing-braces
        -Wreturn-type
        -Wswitch
        -Wswitch-default
        -Wuninitialized
        -Wmissing-field-initializers
        
        $<$<CXX_COMPILER_ID:GNU>:
            -Wmaybe-uninitialized
            
            $<$<VERSION_GREATER:$<CXX_COMPILER_VERSION>,4.8>:
                -Wpedantic
                
                -Wreturn-local-addr
            >
        >
        
        $<$<CXX_COMPILER_ID:Clang>:
            -Wpedantic
            
            # -Wreturn-stack-address # gives false positives
        >
        
        $<$<PLATFORM_ID:Darwin>:
            -pthread
        >
        
        # Required for CMake < 3.1; should be removed if minimum required CMake version is raised.
        $<$<VERSION_LESS:${CMAKE_VERSION},3.1>:
            -std=c++11
        >
    )
endif ()




# Use pthreads on mingw and linux
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    set(DEFAULT_LINKER_OPTIONS
        -pthread
    )
endif()
