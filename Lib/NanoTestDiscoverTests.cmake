# nano_setup_target(TARGET [SOURCES ...] [TARGETS ...] [TEST_PREFIX prefix] [WITH_MAIN])
#
# Sets up an existing executable target for use with NanoTest:
# links NanoTest, optionally adds sources, links additional targets,
# and discovers tests. Pass WITH_MAIN to link NanoTestMain (a prebuilt
# main() that calls nano::run).
#
# Usage:
#   nano_setup_target(MyTests SOURCES tests.cpp TARGETS MyLib)
#   nano_setup_target(MyTests SOURCES tests.cpp WITH_MAIN)

function(nano_setup_target TARGET)
    cmake_parse_arguments(ARG "WITH_MAIN" "TEST_PREFIX" "SOURCES;TARGETS" ${ARGN})

    if(ARG_SOURCES)
        target_sources(${TARGET} PRIVATE ${ARG_SOURCES})
    endif()

    if(ARG_WITH_MAIN)
        target_link_libraries(${TARGET} PRIVATE NanoTestMain)
    else()
        target_link_libraries(${TARGET} PRIVATE NanoTest)
    endif()

    if(ARG_TARGETS)
        target_link_libraries(${TARGET} PRIVATE ${ARG_TARGETS})
    endif()

    nano_discover_tests(${TARGET} TEST_PREFIX "${ARG_TEST_PREFIX}")
endfunction()

# nano_add_executable(TARGET [SOURCES ...] [TARGETS ...] [TEST_PREFIX prefix] [NO_MAIN])
#
# Creates a test executable, links NanoTestMain, and discovers individual
# tests. Pass NO_MAIN to link NanoTest instead (you provide your own main).
#
# Usage:
#   nano_add_executable(MyTests SOURCES tests.cpp)
#   nano_add_executable(MyTests SOURCES tests.cpp NO_MAIN TARGETS MyLib)

function(nano_add_executable TARGET)
    cmake_parse_arguments(ARG "NO_MAIN" "TEST_PREFIX" "SOURCES;TARGETS" ${ARGN})

    add_executable(${TARGET})

    set(_args SOURCES ${ARG_SOURCES} TEST_PREFIX "${ARG_TEST_PREFIX}")
    if(ARG_TARGETS)
        list(APPEND _args TARGETS ${ARG_TARGETS})
    endif()
    if(NOT ARG_NO_MAIN)
        list(APPEND _args WITH_MAIN)
    endif()

    nano_setup_target(${TARGET} ${_args})
endfunction()

# nano_discover_tests(TARGET [TEST_PREFIX prefix])
#
# Discovers individual test cases registered in a NanoTest executable
# and adds each one as a separate CTest test.

function(nano_discover_tests TARGET)
    cmake_parse_arguments(ARG "" "TEST_PREFIX" "" ${ARGN})

    if(CMAKE_CROSSCOMPILING)
        # Can't run the executable on the host when cross-compiling.
        # Fall back to registering the target as a single CTest entry.
        add_test(NAME ${TARGET} COMMAND ${TARGET})
        return()
    endif()

    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
            -D "TEST_PREFIX=${ARG_TEST_PREFIX}"
            -D "CTEST_FILE=${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/NanoTestAddTests.cmake"
        VERBATIM
    )

    # Include the generated CTest file at test time.
    set_property(
        DIRECTORY APPEND PROPERTY
        TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake"
    )
endfunction()
