# Script invoked at build time by nano_discover_tests().
# Runs the test executable with --list-tests and generates add_test() calls:
# one per case (CASE), one per first name segment (PREFIX) or one for the
# whole executable (EXECUTABLE).

# A script run with cmake -P sets no policy version, so on CMake 3.x every
# list() over an empty element warns under CMP0007.
cmake_policy(VERSION 3.31)

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
list(REMOVE_ITEM test_names "")

set(ctest_names "")
file(WRITE "${CTEST_FILE}" "")

function(nano_write_test ctest_name)
    file(APPEND "${CTEST_FILE}" "add_test(\"${ctest_name}\" \"${TEST_EXECUTABLE}\"")

    foreach(arg IN LISTS ARGN)
        file(APPEND "${CTEST_FILE}" " \"${arg}\"")
    endforeach()

    file(APPEND "${CTEST_FILE}" ")\n")

    list(APPEND ctest_names "${ctest_name}")
    set(ctest_names "${ctest_names}" PARENT_SCOPE)
endfunction()

if(TEST_GROUP_BY STREQUAL "EXECUTABLE")
    if(test_names)
        nano_write_test("${TEST_PREFIX}${TEST_TARGET}")
    endif()
elseif(TEST_GROUP_BY STREQUAL "PREFIX")
    set(groups "")

    foreach(test_name IN LISTS test_names)
        string(REGEX REPLACE "/.*" "" group "${test_name}")

        if(group STREQUAL "")
            set(group "${test_name}")
        endif()

        list(APPEND groups "${group}")
    endforeach()

    list(REMOVE_DUPLICATES groups)

    foreach(group IN LISTS groups)
        nano_write_test("${TEST_PREFIX}${group}" --test-group "${group}")
    endforeach()
else()
    foreach(test_name IN LISTS test_names)
        nano_write_test("${TEST_PREFIX}${test_name}" --test "${test_name}")
    endforeach()
endif()

list(JOIN ctest_names "]==] [==[" ctest_name_args)

file(APPEND "${CTEST_FILE}"
    "set([==[${TEST_TARGET}_TESTS]==] [==[${ctest_name_args}]==])\n")
