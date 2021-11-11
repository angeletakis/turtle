cmake_minimum_required(VERSION 3.8.0)
include(FetchContent)

project(turtle		 		VERSION 1.0)
set(PROJECT_DESCRIPTION 		"My Python -> C++ Transcompiler")
set(PROJECT_MAINTAINER_GIT_USERNAME 	"angeletakis")
set(PROJECT_MAINTAINER_NAME 		"Alexios Angel")

file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/include")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/src")
file(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_STANDARD 		20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS		"${CMAKE_CXX_FLAGS} -std=c++${CMAKE_CXX_STANDARD}")
set(CMAKE_BUILD_TYPE		Debug)

include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

file(GLOB_RECURSE PROJECT_SOURCES
     "src/*.h"
     "src/*.cpp"
)

list(REMOVE_ITEM PROJECT_SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

#Generate the static library from the library sources
add_library(LT_SOURCES STATIC 
    "${PROJECT_SOURCES}"
)


#Git outputs a ugly message when downloading the release version of 
execute_process (
    COMMAND 		git config --global --get advice.detachedHead
    OUTPUT_VARIABLE	advice-detachedHead-bool
)

execute_process (
    COMMAND             git config --global advice.detachedHead false
)

message(STATUS "Fetching dependencies")

set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
  ctre
  GIT_REPOSITORY 	 https://github.com/hanickadot/compile-time-regular-expressions.git
  GIT_TAG        	 95c63867bf0f6497825ef6cf44a7d0791bd25883 # v3.4.1
  GIT_PROGRESS		 TRUE
  USES_TERMINAL_DOWNLOAD TRUE
)

FetchContent_MakeAvailable(ctre)

include_directories("${ctre_SOURCE_DIR}/single-header")

if(advice-detachedHead MATCHES "(true)|(false)")
  execute_process (
    COMMAND             git config --global advice.detachedHead "${advice-detachedHead-bool}"
  )
else()
 execute_process (
    COMMAND             git config --global --unset advice.detachedHead
  )
endif()


# Add an executable with the above sources
add_executable(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/src/main.cpp")

target_link_libraries( ${PROJECT_NAME} LT_SOURCES)
