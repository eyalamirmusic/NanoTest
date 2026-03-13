#include <NanoTest/NanoTestMacros.h>

NANO_TEST(TrueIsTrue)
{
    NANO_ASSERT(true);
}

NANO_TEST(ArithmeticEquality)
{
    NANO_ASSERT(2 + 2 == 4);
    NANO_ASSERT(2 + 2 != 5);
}

NANO_TEST(StringEquality)
{
    auto hello = std::string("hello");
    NANO_ASSERT(hello == "hello");
    NANO_ASSERT(hello != "world");
}

NANO_TEST(AssertFalse)
{
    NANO_ASSERT(!false);
    NANO_ASSERT(!0);
}

NANO_TEST_MAIN
