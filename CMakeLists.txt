cmake_minimum_required(VERSION 3.8.0)
include(FetchContent)
include(GNUInstallDirs)

project(turtle VERSION 1.0)
set(PROJECT_DESCRIPTION "My Python -> C++ Transcompiler")
set(PROJECT_MAINTAINER_GIT_USERNAME "angeletakis")
set(PROJECT_MAINTAINER_NAME "Alexios Angel")

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

#vanilla flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")

#warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weverything -fmax-errors=5 -Werror -Wno-c99-designator -Wno-unused-command-line-argument")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-enum-enum-conversion -Wno-deprecated-anon-enum-enum-conversion")

# Debug flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -fno-eliminate-unused-debug-types -fdebug-macro")

# Static Analysis flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --analyze -Xanalyzer -analyzer-output=text")

#required flags to build project
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char -fno-strict-aliasing -fbracket-depth=512 -march=native")

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

#list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

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
        #stacktrace_noop
        )

#Boost Macros to enable libaries
#add_compile_definitions(BOOST_STACKTRACE_LINK)
# boost::stacktrace - requires pthread, backtrace
# boost::log        - requires pthread
add_compile_definitions(BOOST_ENABLE_ASSERT_DEBUG_HANDLER)
add_compile_definitions(BOOST_STACKTRACE_DYN_LINK)
add_compile_definitions(BOOST_LOG_DYN_LINK)
find_package(Threads REQUIRED)
find_package(Backtrace)
# Create a sources variable with a link to all cpp files to compile

# check if boost was found
if (Boost_FOUND)
    message("boost found")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Cannot find Boost")
endif ()

#Git outputs a ugly message when downloading the release version of 
execute_process(
        COMMAND git config --global --get advice.detachedHead
        OUTPUT_VARIABLE advice-detachedHead-bool
)

execute_process(
        COMMAND git config --global advice.detachedHead false
)

message(STATUS "Fetching dependencies")

set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
        ctre
        GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
        GIT_TAG 95c63867bf0f6497825ef6cf44a7d0791bd25883 # v3.4.1
        GIT_PROGRESS TRUE
        USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_MakeAvailable(ctre)

include_directories("${ctre_SOURCE_DIR}/single-header")

if (advice-detachedHead MATCHES "(true)|(false)")
    execute_process(
            COMMAND git config --global advice.detachedHead "${advice-detachedHead-bool}"
    )
else ()
    execute_process(
            COMMAND git config --global --unset advice.detachedHead
    )
endif ()


# Add an executable with the above sources
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/global.cpp
        src/lexicalize.cpp
        src/turtle.cpp

        include/global.h
        include/lexicalize.h
        include/turtle.h
        include/node.h src/node.cpp)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${Threads_LIBRARIES} ${Backtrace_LIBRARIES})
