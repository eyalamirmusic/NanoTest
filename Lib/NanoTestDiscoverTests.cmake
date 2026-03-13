# nano_add_executable(TARGET source1 [source2 ...])
#
# Creates a test executable, links NanoTest, and discovers individual tests.
#
# Usage:
#   nano_add_executable(MyTests tests.cpp)

function(nano_add_executable TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE NanoTest)
    nano_discover_tests(${TARGET})
endfunction()

# nano_discover_tests(TARGET)
#
# Discovers individual test cases registered in a NanoTest executable
# and adds each one as a separate CTest test.

function(nano_discover_tests TARGET)
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
            -D "TEST_PREFIX="
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
