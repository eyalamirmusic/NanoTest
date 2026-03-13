nano_add_executable(NanoTestFunctions
    SOURCES FunctionTestsWithMain.cpp)

add_executable(CustomMain FunctionTests.cpp)
nano_setup_target(CustomMain TEST_PREFIX "MainTests:")
