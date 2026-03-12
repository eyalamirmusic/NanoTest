add_executable(NanoTestBasic BasicTests.cpp)
target_link_libraries(NanoTestBasic PRIVATE NanoTest)

add_test(NAME NanoTestBasic COMMAND NanoTestBasic)
