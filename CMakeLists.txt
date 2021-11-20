cmake_minimum_required(VERSION 3.8.0)

project(turtle VERSION 1.0)
set(PROJECT_DESCRIPTION "My Python -> C++ Transcompiler")
set(PROJECT_MAINTAINER_GIT_USERNAME "angeletakis")
set(PROJECT_MAINTAINER_NAME "Alexios Angel")

# pass the argument -DCMAKE_CXX_COMPILER=/bin/clang++ to cmake if this occurs
if (NOT "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    MESSAGE(FATAL_ERROR "Clang is the only compiler that can compile this source.")
endif()

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(cotire)
include(ucm)

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/deps")

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

#vanilla flags
ucm_add_flags(CXX "-std=c++${CMAKE_CXX_STANDARD}")

# CTRE produces too many warnings :(
# Most of these disabled warnings are a good idea tho
#warnings
ucm_add_flags(CXX
        -Wall -Wextra -Wnewline-eof
        -ferror-limit=5 -fmax-errors=5
        -Wno-c99-designator -Wno-unused-command-line-argument
        -Wno-deprecated-enum-enum-conversion -Wno-deprecated-anon-enum-enum-conversion
        #-Weverything
        #-Wno-c++20-compat -Wno-c++98-compat -Wno-pre-c++17-compat -Wno-c++98-compat-pedantic -Wno-undef
        #-Wno-padded -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wmissing-prototypes
        #-Wno-missing-noreturn -Wno-float-equal -Wno-implicit-int-float-conversion
        #-Wno-sign-conversion -Wno-ctad-maybe-unsupported -Wno-class-varargs
        #-Wno-old-style-cast -Wno-shadow -Wno-reserved-macro-identifier
        )

# Debug flags
ucm_add_flags(CXX
        -ggdb3 -fno-eliminate-unused-debug-types -rdynamic -fdebug-macro
        -disable-llvm-optzns -fno-elide-constructors -fdebug-info-for-profiling
        -fforce-dwarf-frame -gdwarf-5 -fdebug-types-section
        -fdebug-ranges-base-address -gmodules -fstack-usage --verify-debug-info)

#required flags to build project
ucm_add_flags(CXX -fsigned-char -fno-strict-aliasing -fbracket-depth=512 -march=native)

#Generate the static library from the library sources

# find a boost install with the libraries filesystem and system
SET(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.74 REQUIRED
        COMPONENTS
        program_options
        log log_setup
        stacktrace_basic
        stacktrace_backtrace
        #stacktrace_addr2line
        #stacktrace_noop #uncomment when not using stacktrace anymore
        )

#Boost Macros to enable libaries
#add_compile_definitions(BOOST_STACKTRACE_LINK)
# boost::stacktrace - requires pthread, backtrace
# boost::log        - requires pthread
add_compile_definitions(BOOST_ENABLE_ASSERT_DEBUG_HANDLER)
add_compile_definitions(BOOST_STACKTRACE_DYN_LINK)
add_compile_definitions(BOOST_STACKTRACE_USE_BACKTRACE)
add_compile_definitions(BOOST_LOG_DYN_LINK)
find_package(Threads REQUIRED)
find_package(Backtrace)
# Create a sources variable with a link to all cpp files to compile

# check if boost was found
if (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Cannot find Boost")
endif ()

macro(download_src url fpath)
    if(NOT EXISTS "${fpath}")
        file(DOWNLOAD
                "${url}"
                "${fpath}")
    endif()
endmacro()

download_src(
        "https://raw.githubusercontent.com/hanickadot/compile-time-regular-expressions/main/single-header/ctre.hpp"
        "${PROJECT_SOURCE_DIR}/deps/ctre.hpp"
)

download_src(
        "https://raw.githubusercontent.com/Neargye/magic_enum/master/include/magic_enum.hpp"
        "${PROJECT_SOURCE_DIR}/deps/magic_enum.hpp"
)

include_directories("${PROJECT_SOURCE_DIR}/deps")

# Add an executable with the above sources
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/global.cpp
        src/lexicalize.cpp
        src/turtle.cpp
        src/init.cpp
        src/tokenize.cpp
        include/init.hpp
        include/tokenize.hpp
        include/global.hpp
        include/lexicalize.hpp
        include/turtle.hpp
        include/node.hpp
        include/document_types.hpp)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
        CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${Threads_LIBRARIES} ${Backtrace_LIBRARIES})
cotire(${PROJECT_NAME})

#add_subdirectory(tests)
