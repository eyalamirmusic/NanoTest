#pragma once

#include "Printer.h"
#include "Types.h"

#include <source_location>
#include <string_view>
#include <vector>

namespace nano
{
struct Registry
{
    void add(std::string_view nameToUse,
             const std::function<void()>& bodyToUse)
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

    int run(std::string_view filter = {})
    {
        auto filtered = filteredTests(filter);

        Printer::header(filtered.size());

        auto passed = 0;
        auto failed = 0;

        for (auto* test: filtered)
        {
            executeTest(*test);

            if (currentFailed)
            {
                ++failed;
                Printer::failure(test->name, currentFailures);
            }
            else
            {
                ++passed;
                Printer::pass(test->name);
            }
        }

        Printer::summary(passed, failed);
        return failed == 0 ? 0 : 1;
    }

private:
    std::vector<TestCase*> filteredTests(std::string_view filter)
    {
        auto result = std::vector<TestCase*>();

        for (auto& t: tests)
        {
            if (filter.empty() || t.name == filter)
                result.push_back(&t);
        }

        return result;
    }

    void executeTest(TestCase& test)
    {
        currentFailed = false;
        currentFailures.clear();

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
    }

    void recordException(std::string_view what)
    {
        currentFailed = true;
        currentFailures.push_back(
            {"<exception>", 0, "", std::string(what)});
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
