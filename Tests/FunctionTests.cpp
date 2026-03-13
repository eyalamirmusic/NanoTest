#include <NanoTest/NanoTest.h>

static auto checkTrue = nano::test("CheckTrue", [] { nano::check(true); });

static auto checkEquality = nano::test("CheckEquality",
                                       []
                                       {
                                           nano::checkEq(2 + 2, 4);
                                           nano::checkEq(2 + 2, 5);
                                           nano::checkNe(2 + 2, 5);
                                       });

static auto checkStrings = nano::test("CheckStrings",
                                      []
                                      {
                                          auto hello = std::string("hello");
                                          nano::checkEq(hello, std::string("hello"));
                                          nano::checkNe(hello, std::string("world"));
                                      });

static auto checkFalse = nano::test("CheckFalse", [] { nano::checkFalse(false); });

int main()
{
    return nano::run();
}
