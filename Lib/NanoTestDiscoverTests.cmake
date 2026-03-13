# nano_discover_tests(TARGET)
#
# Discovers individual test cases registered in a NanoTest executable
# and adds each one as a separate CTest test.
#
# Usage:
#   add_executable(MyTests tests.cpp)
#   target_link_libraries(MyTests PRIVATE NanoTest)
#   nano_discover_tests(MyTests)

function(nano_discover_tests TARGET)
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
