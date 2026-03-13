add_library(NanoTest STATIC NanoTest/NanoTest.cpp)
target_include_directories(NanoTest PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_library(NanoTestMain STATIC NanoTest/Main.cpp)
target_link_libraries(NanoTestMain PUBLIC NanoTest)

include(${CMAKE_CURRENT_LIST_DIR}/NanoTestDiscoverTests.cmake)
