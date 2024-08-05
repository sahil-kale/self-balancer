cmake_minimum_required(VERSION 3.14)

set(This application_tests)

set(BINARY ${CMAKE_PROJECT_NAME})

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(BUILD_GMOCK ON)

# Add a library with just the sources
#enable Werror, Wextra, Wall, pedantic, and pedantic-errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -Werror -Wextra -Wall")

# enable testing
enable_testing()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test/libs/googletest ${CMAKE_CURRENT_BINARY_DIR}/test/googletest)

add_definitions(-DUNIT_TEST)
# application sources
file(GLOB_RECURSE DUMMY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/dummy/*.cpp)
# print the sources


# Glob recurse the headers inside test/
file(GLOB_RECURSE TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/test/*.hpp)
# Glob recurse the sources inside test/
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../devices)
add_library(MAIN_SOURCES ${DUMMY_SOURCES})
target_compile_options(MAIN_SOURCES PRIVATE -pedantic -pedantic-errors -Wfloat-equal -Wredundant-decls  -Wswitch-default  -pedantic -Wconversion)

add_executable(${This} ${TEST_SOURCES})
#target_compile_options(${This} PUBLIC -pedantic -pedantic-errors)
# Link the executable with the GoogleTest libraries
target_link_libraries(${This} PUBLIC gtest gtest_main gmock MAIN_SOURCES)