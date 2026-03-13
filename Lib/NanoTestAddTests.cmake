# Script invoked at build time by nano_discover_tests().
# Runs the test executable with --list-tests and generates add_test() calls.

execute_process(
    COMMAND "${TEST_EXECUTABLE}" --list-tests
    OUTPUT_VARIABLE test_list
    RESULT_VARIABLE result
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Failed to list tests from ${TEST_EXECUTABLE}")
endif()

# Convert newline-separated output to a CMake list.
string(REPLACE "\n" ";" test_names "${test_list}")

# Write a CTest file with one add_test() per test case.
file(WRITE "${CTEST_FILE}" "")
foreach(test_name IN LISTS test_names)
    if(NOT test_name STREQUAL "")
        file(APPEND "${CTEST_FILE}"
            "add_test(\"${TEST_PREFIX}${test_name}\" \"${TEST_EXECUTABLE}\" --test \"${test_name}\")\n"
        )
    endif()
endforeach()
