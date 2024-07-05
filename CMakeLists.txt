cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(red-black-tree)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

set(TestSrc
    tests/main.cpp
    tests/tests.cpp
)

set(EXECUTABLE_NAME rb-tree-test.out)

add_executable(${EXECUTABLE_NAME} ${TestSrc})

target_link_libraries(
    ${EXECUTABLE_NAME}
    GTest::gtest_main
)

target_include_directories(
    ${EXECUTABLE_NAME}
    PRIVATE src
)

enable_testing()
include(GoogleTest)
