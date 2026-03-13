#include <NanoTest/NanoTest.h>

using namespace nano;

auto checkTrue = test("CheckTrueA") = [] { check(true); };

auto checkEquality = test("CheckEqualityB") = []
{
    check(2 + 2 == 4);
    check(2 + 2 != 5);
};

auto checkStrings = test("CheckStringsC") = []
{
    auto hello = std::string("hello");
    check(hello == "hello");
    check(hello != "world");
};

auto checkFalse = test("CheckFalseD") = [] { check(!false); };

int main(int argc, char* argv[])
{
    return run(argc, argv);
}
