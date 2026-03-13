#include <NanoTest/NanoTest.h>

static auto checkTrue = nano::test("CheckTrue", [] { nano::check(true); });

static auto checkEquality = nano::test("CheckEquality",
                                       []
                                       {
                                           nano::check(2 + 2 == 4);
                                           nano::check(2 + 2 != 5);
                                       });

static auto checkStrings = nano::test("CheckStrings",
                                      []
                                      {
                                          auto hello = std::string("hello");
                                          nano::check(hello == "hello");
                                          nano::check(hello != "world");
                                      });

static auto checkFalse = nano::test("CheckFalse", [] { nano::check(!false); });

int main()
{
    return nano::run();
}
