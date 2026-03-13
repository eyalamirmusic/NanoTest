include(${CMAKE_SOURCE_DIR}/Lib/NanoTestDiscoverTests.cmake)

add_executable(NanoTestBasic BasicTests.cpp)
target_link_libraries(NanoTestBasic PRIVATE NanoTest)
nano_discover_tests(NanoTestBasic)

add_executable(NanoTestFunctions FunctionTests.cpp)
target_link_libraries(NanoTestFunctions PRIVATE NanoTest)
nano_discover_tests(NanoTestFunctions)
