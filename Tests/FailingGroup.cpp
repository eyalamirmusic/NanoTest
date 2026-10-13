#include <NanoTest/NanoTest.h>

using namespace nano;

auto before = test("Failing/passesBefore") = [] { check(true); };
auto fails = test("Failing/failsOnPurpose") = [] { check(1 + 1 == 3); };
auto after = test("Failing/passesAfter") = [] { check(true); };
auto elsewhere = test("Other/neverRunsInTheGroup") = [] { check(false); };
