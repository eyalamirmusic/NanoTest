add_executable(NanoTestBasic BasicTests.cpp)
target_link_libraries(NanoTestBasic PRIVATE NanoTest)
add_test(NAME NanoTestBasic COMMAND NanoTestBasic)

add_executable(NanoTestFunctions FunctionTests.cpp)
target_link_libraries(NanoTestFunctions PRIVATE NanoTest)
add_test(NAME NanoTestFunctions COMMAND NanoTestFunctions)
