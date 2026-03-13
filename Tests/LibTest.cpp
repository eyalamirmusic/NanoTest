#include "Lib.h"
#include "NanoTest/NanoTest.h"

using namespace nano;

auto checkLib = test("CheckLib") = [] { check(Lib::get1() == 1); };
