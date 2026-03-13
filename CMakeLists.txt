cmake_minimum_required(VERSION 3.21)
project(NanoTest)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Lib)

option(NANO_BUILD_TESTS "Build NanoTest internal tests" OFF)

if(PROJECT_IS_TOP_LEVEL OR NANO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()