#pragma once

#include <functional>
#include <source_location>
#include <string>
#include <string_view>

namespace nano
{

// Register a test. Returns a dummy value for use with static auto.
bool test(std::string name, const std::function<void()>& body);

// Run all registered tests. Returns 0 on success, 1 on failure.
int run();

// Assert that expr is true.
void check(bool expr,
           std::string_view exprStr = {},
           const std::source_location& loc = std::source_location::current());

} // namespace nano

// ---------------------------------------------------------------------------
// Macros (thin wrappers that add expression stringification)
// ---------------------------------------------------------------------------

#define NANO_TEST(name)                                                             \
    static void nanoTest_##name();                                                  \
    static auto nanoReg_##name = ::nano::test(#name, nanoTest_##name);              \
    static void nanoTest_##name()

#define NANO_ASSERT(expr) ::nano::check((expr), #expr)

#define NANO_TEST_MAIN                                                              \
    int main()                                                                      \
    {                                                                               \
        return ::nano::run();                                                       \
    }
