#include <NanoTest/NanoTest.h>

auto checkTrue = nano::test("CheckTrue", [] { nano::check(true); });

auto checkEquality = nano::test("CheckEquality",
                                []
                                {
                                    nano::check(2 + 2 == 4);
                                    nano::check(2 + 2 != 5);
                                });

auto checkStrings = nano::test("CheckStrings",
                               []
                               {
                                   auto hello = std::string("hello");
                                   nano::check(hello == "hello");
                                   nano::check(hello != "world");
                               });

auto checkFalse = nano::test("CheckFalse", [] { nano::check(!false); });

int main()
{
    return nano::run();
}
