#pragma once

#include <functional>
#include <source_location>
#include <string>
#include <string_view>

namespace nano
{

struct RunOptions
{
    std::string test;
    bool listTests = false;
};

// Register a test. Returns a dummy value for use with static auto.
bool test(std::string name, const std::function<void()>& body);

// Run all registered tests. Returns 0 on success, 1 on failure.
// Supports --list-tests and --test <name> when argc/argv are provided.
int run(int argc = 0, char* argv[] = nullptr);

// Same as the argc/argv version but supports manual argument passing:
int run(const RunOptions& options = {});

// Assert that expr is true.
void check(bool expr,
           std::string_view exprStr = {},
           const std::source_location& loc = std::source_location::current());

} // namespace nano
