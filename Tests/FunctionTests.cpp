#include <NanoTest/NanoTest.h>

auto checkTrue = nano::test("CheckTrueA", [] { nano::check(true); });

auto checkEquality = nano::test("CheckEqualityB",
                                []
                                {
                                    nano::check(2 + 2 == 4);
                                    nano::check(2 + 2 != 5);
                                });

auto checkStrings = nano::test("CheckStringsC",
                               []
                               {
                                   auto hello = std::string("hello");
                                   nano::check(hello == "hello");
                                   nano::check(hello != "world");
                               });

auto checkFalse = nano::test("CheckFalseD", [] { nano::check(!false); });

int main(int argc, char* argv[])
{
    return nano::run(argc, argv);
}
