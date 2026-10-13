#pragma once

#include "NanoTest.h"
#include "Printer.h"
#include "Types.h"

#include <source_location>
#include <string_view>
#include <vector>

namespace nano
{
constexpr bool areExceptionsEnabled()
{
#if __cpp_exceptions
    return true;
#else
    return false;
#endif
}

struct Registry
{
    void add(std::string_view nameToUse, const std::function<void()>& bodyToUse)
    {
        tests.push_back({std::string(nameToUse), bodyToUse});
    }

    void fail(const std::source_location& loc,
              std::string_view expression,
              std::string_view message = {})
    {
        currentFailed = true;
        currentFailures.push_back({loc.file_name(),
                                   static_cast<int>(loc.line()),
                                   std::string(expression),
                                   std::string(message)});
    }

    void listTests()
    {
        for (auto& [name, body]: tests)
            std::cout << name << '\n';
    }

    int run(const RunOptions& options = {})
    {
        auto filtered = filteredTests(options);

        Printer::header(filtered.size());

        auto passed = 0;
        auto failedNames = std::vector<std::string_view>();

        for (auto* test: filtered)
        {
            executeTest(*test);

            if (currentFailed)
            {
                failedNames.push_back(test->name);
                Printer::failure(test->name, currentFailures);
            }
            else
            {
                ++passed;
                Printer::pass(test->name);
            }
        }

        Printer::summary(passed, failedNames);
        return failedNames.empty() ? 0 : 1;
    }

    static bool isInGroup(std::string_view name, std::string_view group)
    {
        if (group.ends_with('/'))
            group.remove_suffix(1);

        if (!name.starts_with(group))
            return false;

        return name.size() == group.size() || name[group.size()] == '/';
    }

private:
    static bool isSelected(const TestCase& test, const RunOptions& options)
    {
        if (options.test.empty() && options.group.empty())
            return true;

        if (!options.test.empty() && test.name == options.test)
            return true;

        return !options.group.empty() && isInGroup(test.name, options.group);
    }

    std::vector<TestCase*> filteredTests(const RunOptions& options)
    {
        auto result = std::vector<TestCase*>();

        for (auto& t: tests)
        {
            if (isSelected(t, options))
                result.push_back(&t);
        }

        return result;
    }

    void executeTest(TestCase& test)
    {
        currentFailed = false;
        currentFailures.clear();

#if __cpp_exceptions
        try
        {
            test.body();
        }
        catch (const std::exception& e)
        {
            recordException(e.what());
        }
        catch (...)
        {
            recordException("unknown exception");
        }
#else
        test.body();
#endif
    }

    void recordException(std::string_view what)
    {
        currentFailed = true;
        currentFailures.push_back({"<exception>", 0, "", std::string(what)});
    }

    std::vector<TestCase> tests;
    bool currentFailed = false;
    std::vector<TestFailure> currentFailures;
};

inline Registry& getRegistry()
{
    static auto reg = Registry();
    return reg;
}

} // namespace nano
