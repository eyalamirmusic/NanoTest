#pragma once

#include "NanoTest.h"

#define NANO_TEST(name)                                                             \
    static void nanoTest_##name();                                                  \
    static auto nanoReg_##name = ::nano::test(#name, nanoTest_##name);              \
    static void nanoTest_##name()

#define NANO_ASSERT(expr) ::nano::check((expr), #expr)

#define NANO_TEST_MAIN                                                              \
    int main(int argc, char* argv[])                                                \
    {                                                                               \
        return ::nano::run(argc, argv);                                             \
    }
