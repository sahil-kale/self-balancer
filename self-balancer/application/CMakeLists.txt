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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/comm)
file(GLOB_RECURSE COMM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/comm/*.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/telem)
file(GLOB_RECURSE TELEMETRY_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/telem/*.cpp)

# Message sources - located at ../generated/messages/**/*.c
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../generated)


# include nanopb, located at ../nanopb/
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../nanopb)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../devices)

add_library(MAIN_SOURCES ${DUMMY_SOURCES} ${COMM_SOURCES} ${TELEMETRY_SOURCES} )
target_compile_options(MAIN_SOURCES PRIVATE -pedantic -pedantic-errors -Wfloat-equal -Wredundant-decls  -Wswitch-default  -pedantic -Wconversion)

# Glob recurse the sources inside test/
file(GLOB_RECURSE TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)
# compile nanopb sources
file(GLOB NANOPB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../nanopb/*.c)

file(GLOB_RECURSE MESSAGE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../generated/messages/**/*.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/test/mocks)
add_executable(${This} ${TEST_SOURCES} ${NANOPB_SOURCES} ${MESSAGE_SOURCES})
#target_compile_options(${This} PUBLIC -pedantic -pedantic-errors)
# Link the executable with the GoogleTest libraries
target_link_libraries(${This} PUBLIC gtest gtest_main gmock MAIN_SOURCES)