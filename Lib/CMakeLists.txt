add_library(NanoTest STATIC NanoTest/NanoTest.cpp)
target_include_directories(NanoTest PUBLIC ${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/NanoTestDiscoverTests.cmake)
