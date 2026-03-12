#include <NanoTest/NanoTest.h>

NANO_TEST(TrueIsTrue)
{
    NANO_ASSERT(true);
}

NANO_TEST(ArithmeticEquality)
{
    NANO_ASSERT_EQ(2 + 2, 4);
    NANO_ASSERT_NE(2 + 2, 5);
}

NANO_TEST(StringEquality)
{
    std::string hello = "hello";
    NANO_ASSERT_EQ(hello, std::string("hello"));
    NANO_ASSERT_NE(hello, std::string("world"));
}

NANO_TEST(AssertFalse)
{
    NANO_ASSERT_FALSE(false);
    NANO_ASSERT_FALSE(0);
}

NANO_TEST_MAIN
