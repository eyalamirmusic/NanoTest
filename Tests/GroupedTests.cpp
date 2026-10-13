#include <NanoTest/NanoTest.h>

using namespace nano;

auto alpha = test("Alpha") = [] { check(true); };
auto alphaOne = test("Alpha/one") = [] { check(true); };
auto alphaSubTwo = test("Alpha/sub/two") = [] { check(true); };
auto alphaBetaOne = test("AlphaBeta/one") = [] { check(true); };
auto betaOne = test("Beta/one") = [] { check(true); };
