nano_add_executable(NanoTestFunctions
    SOURCES FunctionTestsWithMain.cpp)

add_executable(CustomMain FunctionTests.cpp)
nano_setup_target(CustomMain TEST_PREFIX "MainTests:")

add_subdirectory(DummyLibraryUnderTest)
nano_add_executable(LibTest.cpp SOURCES LibTest.cpp TARGETS DummyLibraryUnderTest)