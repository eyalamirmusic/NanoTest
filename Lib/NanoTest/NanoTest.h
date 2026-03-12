#pragma once

#include <concepts>
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

    // Called by assertion macros to report a failure.
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

} // namespace nano

// ---------------------------------------------------------------------------
// Macros
// ---------------------------------------------------------------------------

#define NANO_TEST(name)                                                             \
    static void nanoTest_##name();                                                  \
    static ::nano::AutoRegister nanoReg_##name(#name, nanoTest_##name);             \
    static void nanoTest_##name()

#define NANO_ASSERT(expr)                                                           \
    do                                                                              \
    {                                                                               \
        if (!(expr))                                                                \
        {                                                                           \
            ::nano::Registry::instance().fail(std::source_location::current(),      \
                                              #expr);                               \
        }                                                                           \
    } while (false)

#define NANO_ASSERT_FALSE(expr)                                                     \
    do                                                                              \
    {                                                                               \
        if ((expr))                                                                 \
        {                                                                           \
            ::nano::Registry::instance().fail(std::source_location::current(),      \
                                              "!(" #expr ")");                      \
        }                                                                           \
    } while (false)

#define NANO_ASSERT_EQ(a, b)                                                        \
    do                                                                              \
    {                                                                               \
        if (!((a) == (b)))                                                          \
        {                                                                           \
            ::nano::Registry::instance().fail(std::source_location::current(),      \
                                              #a " == " #b);                        \
        }                                                                           \
    } while (false)

#define NANO_ASSERT_NE(a, b)                                                        \
    do                                                                              \
    {                                                                               \
        if (!((a) != (b)))                                                          \
        {                                                                           \
            ::nano::Registry::instance().fail(std::source_location::current(),      \
                                              #a " != " #b);                        \
        }                                                                           \
    } while (false)

// Provide a main() that runs all registered tests.
#define NANO_TEST_MAIN                                                              \
    int main()                                                                      \
    {                                                                               \
        return ::nano::Registry::instance().run();                                  \
    }
