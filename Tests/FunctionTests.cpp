#include <NanoTest/NanoTest.h>
#include <string>

using namespace nano;

auto checkTrue = test("CheckTrue") = [] { check(true); };

auto checkEquality = test("CheckEquality") = []
{
    check(2 + 2 == 4);
    check(2 + 2 != 5);
};

auto checkStrings = test("CheckStrings") = []
{
    auto hello = std::string("hello");
    check(hello == "hello");
    check(hello != "world");
};

auto checkFalse = test("CheckFalse") = [] { check(!false); };

int main(int argc, char* argv[])
{
    return run(argc, argv);
}
