nano_add_executable(NanoTestFunctions
    SOURCES FunctionTestsWithMain.cpp)

add_executable(CustomMain FunctionTests.cpp)
nano_setup_target(CustomMain TEST_PREFIX "MainTests:")

add_subdirectory(DummyLibraryUnderTest)
nano_add_executable(LibTest.cpp SOURCES LibTest.cpp TARGETS DummyLibraryUnderTest)

nano_add_executable(NanoTestFilters SOURCES FilterTests.cpp GROUP_BY PREFIX)

nano_add_executable(GroupedByCase
    SOURCES GroupedTests.cpp
    TEST_PREFIX "Case:")

nano_add_executable(GroupedByPrefix
    SOURCES GroupedTests.cpp
    TEST_PREFIX "Prefix:"
    GROUP_BY PREFIX
    PROPERTIES LABELS grouped RESOURCE_LOCK "lockA;lockB")

nano_add_executable(GroupedWhole
    SOURCES GroupedTests.cpp
    TEST_PREFIX "Whole:"
    GROUP_BY EXECUTABLE
    PROPERTIES LABELS whole)

if(NOT CMAKE_CROSSCOMPILING)
    add_executable(FailingGroup FailingGroup.cpp)
    target_link_libraries(FailingGroup PRIVATE NanoTestMain)

    add_test(NAME CheckDiscovery
        COMMAND ${CMAKE_COMMAND}
            -D "CTEST_COMMAND=${CMAKE_CTEST_COMMAND}"
            -D "TEST_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            -D "FAILING_GROUP=$<TARGET_FILE:FailingGroup>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/CheckDiscovery.cmake")
endif()
