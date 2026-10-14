# Checks what nano_discover_tests registered in this directory, and how a
# grouped run with a failing case reports it. Run by CTest as CheckDiscovery.

set(errors "")

macro(report message_text)
    string(APPEND errors "\n  ${message_text}")
endmacro()

execute_process(
    COMMAND "${CTEST_COMMAND}" --test-dir "${TEST_DIR}" --show-only=json-v1
    OUTPUT_VARIABLE json
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "ctest --show-only failed in ${TEST_DIR}")
endif()

function(json_array_as_text json_array out_var)
    set(text "")
    string(JSON length LENGTH "${json_array}")

    foreach(i RANGE 1 ${length})
        math(EXPR index "${i} - 1")
        string(JSON element GET "${json_array}" ${index})
        string(APPEND text " ${element}")
    endforeach()

    string(STRIP "${text}" text)
    set(${out_var} "${text}" PARENT_SCOPE)
endfunction()

set(names "")
string(JSON test_count LENGTH "${json}" tests)

foreach(i RANGE 1 ${test_count})
    math(EXPR index "${i} - 1")
    string(JSON name GET "${json}" tests ${index} name)
    string(JSON command GET "${json}" tests ${index} command)
    string(JSON properties GET "${json}" tests ${index} properties)

    string(JSON command_length LENGTH "${command}")
    set(arguments_text "")

    foreach(argument_index RANGE 1 ${command_length})
        if(argument_index LESS command_length)
            string(JSON argument GET "${command}" ${argument_index})
            string(APPEND arguments_text " ${argument}")
        endif()
    endforeach()

    string(STRIP "${arguments_text}" arguments_text)

    string(MAKE_C_IDENTIFIER "${name}" key)
    list(APPEND names "${name}")
    set("arguments_${key}" "${arguments_text}")
    set("properties_${key}" "${properties}")
endforeach()

function(arguments_of test_name out_var)
    string(MAKE_C_IDENTIFIER "${test_name}" key)
    set(${out_var} "${arguments_${key}}" PARENT_SCOPE)
endfunction()

function(property_of test_name property out_var)
    set(value "")
    string(MAKE_C_IDENTIFIER "${test_name}" key)
    set(properties "${properties_${key}}")
    string(JSON count LENGTH "${properties}")

    foreach(i RANGE 1 ${count})
        math(EXPR index "${i} - 1")
        string(JSON property_name GET "${properties}" ${index} name)

        if(property_name STREQUAL property)
            string(JSON type TYPE "${properties}" ${index} value)
            string(JSON value GET "${properties}" ${index} value)

            if(type STREQUAL "ARRAY")
                json_array_as_text("${value}" value)
            endif()
        endif()
    endforeach()

    set(${out_var} "${value}" PARENT_SCOPE)
endfunction()

function(entries_with_prefix prefix out_var)
    set(matching "")

    foreach(name IN LISTS names)
        string(FIND "${name}" "${prefix}" position)

        if(position EQUAL 0)
            string(APPEND matching " ${name}")
        endif()
    endforeach()

    string(STRIP "${matching}" matching)
    set(${out_var} "${matching}" PARENT_SCOPE)
endfunction()

# PREFIX: one entry per first segment, in registration order.
entries_with_prefix("Prefix:" prefix_entries)

if(NOT prefix_entries STREQUAL "Prefix:Alpha Prefix:AlphaBeta Prefix:Beta")
    report("PREFIX entries are Alpha, AlphaBeta, Beta, got '${prefix_entries}'")
endif()

arguments_of("Prefix:Alpha" alpha_arguments)

if(NOT alpha_arguments STREQUAL "--test-group Alpha")
    report("Prefix:Alpha runs '${alpha_arguments}'")
endif()

arguments_of("Prefix:AlphaBeta" alpha_beta_arguments)

if(NOT alpha_beta_arguments STREQUAL "--test-group AlphaBeta")
    report("Prefix:AlphaBeta runs '${alpha_beta_arguments}'")
endif()

string(REPLACE " " ";" prefix_entry_list "${prefix_entries}")

foreach(entry IN LISTS prefix_entry_list)
    property_of("${entry}" LABELS labels)
    property_of("${entry}" RESOURCE_LOCK locks)

    if(NOT labels STREQUAL "grouped")
        report("${entry} has LABELS '${labels}', expected 'grouped'")
    endif()

    if(NOT locks STREQUAL "lockA lockB")
        report("${entry} has RESOURCE_LOCK '${locks}', expected 'lockA lockB'")
    endif()
endforeach()

# EXECUTABLE: one entry named after the target, with no filter.
entries_with_prefix("Whole:" whole_entries)

if(NOT whole_entries STREQUAL "Whole:GroupedWhole")
    report("EXECUTABLE entry is Whole:GroupedWhole, got '${whole_entries}'")
endif()

arguments_of("Whole:GroupedWhole" whole_arguments)

if(NOT whole_arguments STREQUAL "")
    report("Whole:GroupedWhole runs '${whole_arguments}'")
endif()

property_of("Whole:GroupedWhole" LABELS whole_labels)

if(NOT whole_labels STREQUAL "whole")
    report("Whole:GroupedWhole has LABELS '${whole_labels}', expected 'whole'")
endif()

# CASE, the default: one entry per case, as before.
entries_with_prefix("Case:" case_entries)
string(REPLACE " " ";" case_entry_list "${case_entries}")
list(LENGTH case_entry_list case_count)

if(NOT case_count EQUAL 5)
    report("CASE registers 5 entries, got '${case_entries}'")
endif()

arguments_of("Case:Alpha/sub/two" case_arguments)

if(NOT case_arguments STREQUAL "--test Alpha/sub/two")
    report("Case:Alpha/sub/two runs '${case_arguments}'")
endif()

# Discovery itself, rerun for each mode: no warning, and the same entries the
# POST_BUILD step wrote.
foreach(mode_target IN ITEMS
        "CASE|Case:|GroupedByCase|${GROUPED_BY_CASE}"
        "PREFIX|Prefix:|GroupedByPrefix|${GROUPED_BY_PREFIX}"
        "EXECUTABLE|Whole:|GroupedWhole|${GROUPED_WHOLE}")
    string(REPLACE "|" ";" mode_target "${mode_target}")
    list(GET mode_target 0 mode)
    list(GET mode_target 1 prefix)
    list(GET mode_target 2 target)
    list(GET mode_target 3 executable)

    set(rediscovered "${TEST_DIR}/${target}_rediscovered.cmake")

    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_EXECUTABLE=${executable}"
            -D "TEST_TARGET=${target}"
            -D "TEST_PREFIX=${prefix}"
            -D "TEST_GROUP_BY=${mode}"
            -D "CTEST_FILE=${rediscovered}"
            -P "${ADD_TESTS_SCRIPT}"
        OUTPUT_VARIABLE discovery_output
        ERROR_VARIABLE discovery_output
        RESULT_VARIABLE result
    )

    if(NOT result EQUAL 0)
        report("${mode} discovery failed: ${discovery_output}")
    endif()

    string(FIND "${discovery_output}" "Warning" position)

    if(NOT position EQUAL -1)
        report("${mode} discovery warned: ${discovery_output}")
    endif()

    file(READ "${rediscovered}" rediscovered_text)
    file(READ "${TEST_DIR}/${target}_tests.cmake" registered_text)

    if(NOT rediscovered_text STREQUAL registered_text)
        report("${mode} discovery wrote:\n${rediscovered_text}")
    endif()
endforeach()

# A grouped run with a failing case runs the rest and names the failure.
execute_process(
    COMMAND "${FAILING_GROUP}" --test-group Failing
    OUTPUT_VARIABLE output
    RESULT_VARIABLE result
)

if(result EQUAL 0)
    report("a group with a failing case exited 0")
endif()

foreach(expected_text
        "PASS  Failing/passesBefore"
        "FAIL  Failing/failsOnPurpose"
        "PASS  Failing/passesAfter"
        "2 passed, 1 failed, 3 total"
        "Failed tests:\n  Failing/failsOnPurpose\n")
    string(FIND "${output}" "${expected_text}" position)

    if(position EQUAL -1)
        report("the grouped output lacks '${expected_text}'")
    endif()
endforeach()

string(FIND "${output}" "Other/" position)

if(NOT position EQUAL -1)
    report("the group ran a case outside it")
endif()

if(errors)
    message(FATAL_ERROR "Discovery checks failed:${errors}\n\nOutput:\n${output}")
endif()

message(STATUS "All discovery checks passed")
