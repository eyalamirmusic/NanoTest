# nano_setup_target(TARGET [SOURCES ...] [TARGETS ...] [TEST_PREFIX prefix]
#                   [GROUP_BY CASE|PREFIX|EXECUTABLE]
#                   [PROPERTIES name value ...] [WITH_MAIN])
#
# Sets up an existing executable target for use with NanoTest:
# links NanoTest, optionally adds sources, links additional targets,
# and discovers tests. Pass WITH_MAIN to link NanoTestMain (a prebuilt
# main() that calls nano::run). The discovery options are those of
# nano_discover_tests.
#
# Usage:
#   nano_setup_target(MyTests SOURCES tests.cpp TARGETS MyLib)
#   nano_setup_target(MyTests SOURCES tests.cpp WITH_MAIN)

function(nano_setup_target TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG
        "WITH_MAIN" "TEST_PREFIX;GROUP_BY" "SOURCES;TARGETS;PROPERTIES")

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

    nano_discover_tests(${TARGET}
        TEST_PREFIX "${ARG_TEST_PREFIX}"
        GROUP_BY "${ARG_GROUP_BY}"
        PROPERTIES ${ARG_PROPERTIES})
endfunction()

# nano_add_executable(TARGET [SOURCES ...] [TARGETS ...] [TEST_PREFIX prefix]
#                     [GROUP_BY CASE|PREFIX|EXECUTABLE]
#                     [PROPERTIES name value ...] [NO_MAIN])
#
# Creates a test executable, links NanoTestMain, and discovers individual
# tests. Pass NO_MAIN to link NanoTest instead (you provide your own main).
# The discovery options are those of nano_discover_tests.
#
# Usage:
#   nano_add_executable(MyTests SOURCES tests.cpp)
#   nano_add_executable(MyTests SOURCES tests.cpp NO_MAIN TARGETS MyLib)

function(nano_add_executable TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG
        "NO_MAIN" "TEST_PREFIX;GROUP_BY" "SOURCES;TARGETS;PROPERTIES")

    add_executable(${TARGET})

    set(_args SOURCES ${ARG_SOURCES})
    if(ARG_TARGETS)
        list(APPEND _args TARGETS ${ARG_TARGETS})
    endif()
    if(NOT ARG_NO_MAIN)
        list(APPEND _args WITH_MAIN)
    endif()

    nano_setup_target(${TARGET} ${_args}
        TEST_PREFIX "${ARG_TEST_PREFIX}"
        GROUP_BY "${ARG_GROUP_BY}"
        PROPERTIES ${ARG_PROPERTIES})
endfunction()

# nano_discover_tests(TARGET [TEST_PREFIX prefix]
#                     [GROUP_BY CASE|PREFIX|EXECUTABLE]
#                     [PROPERTIES name value ...])
#
# Discovers the test cases registered in a NanoTest executable and adds
# them to CTest after every build. GROUP_BY decides how many processes a
# run takes:
#
#   CASE        one CTest entry per case, running '--test <name>' (default).
#   PREFIX      one entry per first '/'-separated segment of the case names,
#               named after it and running '--test-group <segment>', which
#               runs the case named exactly that and every case under it:
#               'Net', 'Net/get' and 'Net/Socket/open' are the one entry 'Net'.
#   EXECUTABLE  one entry for the whole executable, named after the target.
#
# Entry names are TEST_PREFIX followed by the case, the group or the target
# name. When GROUP_BY is not given, the NANO_TEST_GROUP_BY variable is used,
# so a project can switch all of its executables at once.
#
# PROPERTIES are applied to every entry this call creates, through
# set_tests_properties; list values are passed whole. The entry names are
# also in the CTest-time variable <TARGET>_TESTS, for include files of your
# own appended to TEST_INCLUDE_FILES after this call.

function(nano_discover_tests TARGET)
    cmake_parse_arguments(PARSE_ARGV 1 ARG
        "" "TEST_PREFIX;GROUP_BY" "PROPERTIES")

    if(NOT ARG_GROUP_BY)
        set(ARG_GROUP_BY "${NANO_TEST_GROUP_BY}")
    endif()

    if(NOT ARG_GROUP_BY)
        set(ARG_GROUP_BY CASE)
    endif()

    if(NOT ARG_GROUP_BY MATCHES "^(CASE|PREFIX|EXECUTABLE)$")
        message(FATAL_ERROR
            "nano_discover_tests: GROUP_BY must be CASE, PREFIX or EXECUTABLE, "
            "not '${ARG_GROUP_BY}'")
    endif()

    if(CMAKE_CROSSCOMPILING)
        # Can't run the executable on the host when cross-compiling.
        # Fall back to registering the target as a single CTest entry.
        add_test(NAME ${TARGET} COMMAND ${TARGET})
        return()
    endif()

    set(_ctest_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake")

    # Placeholder so IDEs (notably CLion) see the test target at configure
    # time, before the first build. POST_BUILD replaces it with the real
    # entries. The EXISTS guard prevents a reconfigure from clobbering
    # the real entries when the target is already up-to-date and POST_BUILD
    # won't re-run. The grouping options are on the POST_BUILD command line,
    # so changing them relinks the target and discovery runs again.
    if(NOT EXISTS "${_ctest_file}")
        file(WRITE "${_ctest_file}"
            "add_test(${TARGET} \"${CMAKE_COMMAND}\" -E true)\n"
            "set([==[${TARGET}_TESTS]==] [==[${TARGET}]==])\n")
    endif()

    add_custom_command(
        TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
            -D "TEST_TARGET=${TARGET}"
            -D "TEST_PREFIX=${ARG_TEST_PREFIX}"
            -D "TEST_GROUP_BY=${ARG_GROUP_BY}"
            -D "CTEST_FILE=${_ctest_file}"
            -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/NanoTestAddTests.cmake"
        VERBATIM
    )

    # Include the generated CTest file at test time.
    set_property(
        DIRECTORY APPEND PROPERTY
        TEST_INCLUDE_FILES "${_ctest_file}"
    )

    if(ARG_PROPERTIES)
        set(_properties_file
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_test_properties.cmake")

        set(_property_args "")
        foreach(_property IN LISTS ARG_PROPERTIES)
            string(APPEND _property_args " [==[${_property}]==]")
        endforeach()

        file(WRITE "${_properties_file}"
            "if(${TARGET}_TESTS)\n"
            "    set_tests_properties(\${${TARGET}_TESTS} PROPERTIES"
            "${_property_args})\n"
            "endif()\n")

        set_property(
            DIRECTORY APPEND PROPERTY
            TEST_INCLUDE_FILES "${_properties_file}"
        )
    endif()
endfunction()
