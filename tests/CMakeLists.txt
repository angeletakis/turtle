# 'Google_test' is the subproject name
project(tests)
include(FetchContent)

file(MAKE_DIRECTORY "${tests_SOURCE_DIR}/bin")

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)
enable_testing()
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include_directories(${googletest_INCLUDE_DIR} ${googletest_SOURCE_DIR})


add_executable(tests hello_test.cpp)
set_target_properties(tests
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${tests_SOURCE_DIR}/bin")
target_link_libraries(tests gtest gtest_main ${googletest_LIBRARIES})
include(GoogleTest)
gtest_discover_tests(tests)
#add_test(test-1 tests)
