#pragma once

#include <functional>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace nano
{

struct TestFailure
{
    std::string file;
    int line;
    std::string expression;
    std::string message;
};

struct TestCase
{
    std::string name;
    std::function<void()> body;
};

class Registry
{
public:
    static Registry& instance();

    void add(std::string name, std::function<void()> body);
    int run();

    void fail(const std::source_location& loc,
              std::string_view expression,
              std::string_view message = {});

    [[nodiscard]] bool currentTestFailed() const { return currentFailed; }

private:
    std::vector<TestCase> tests;
    bool currentFailed = false;
    std::vector<TestFailure> currentFailures;
};

// RAII helper for auto-registration.
struct AutoRegister
{
    AutoRegister(std::string nameToUse, std::function<void()> bodyToUse)
    {
        Registry::instance().add(std::move(nameToUse), std::move(bodyToUse));
    }
};

// ---------------------------------------------------------------------------
// Free functions (macro-free API)
// ---------------------------------------------------------------------------

// Register a test. Returns a dummy value for use with static auto.
inline auto test(std::string name, std::function<void()> body) -> bool
{
    Registry::instance().add(std::move(name), std::move(body));
    return true;
}

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
    static ::nano::AutoRegister nanoReg_##name(#name, nanoTest_##name);             \
    static void nanoTest_##name()

#define NANO_ASSERT(expr) ::nano::check((expr), #expr)

#define NANO_TEST_MAIN                                                              \
    int main()                                                                      \
    {                                                                               \
        return ::nano::run();                                                       \
    }
