include(${CMAKE_SOURCE_DIR}/Lib/NanoTestDiscoverTests.cmake)

nano_add_executable(NanoTestBasic BasicTests.cpp)
nano_add_executable(NanoTestFunctions FunctionTests.cpp)
