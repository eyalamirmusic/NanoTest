#include "NanoTest.h"

#include <iostream>
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

struct Registry
{
    static Registry& instance()
    {
        static auto reg = Registry();
        return reg;
    }

    void add(std::string nameToUse, const std::function<void()>& bodyToUse)
    {
        tests.push_back({std::move(nameToUse), bodyToUse});
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

        printHeader(filtered.size());

        auto passed = 0;
        auto failed = 0;

        for (auto* test: filtered)
        {
            executeTest(*test);

            if (currentFailed)
            {
                ++failed;
                printFailure(test->name);
            }
            else
            {
                ++passed;
                printPass(test->name);
            }
        }

        printSummary(passed, failed);
        return failed == 0 ? 0 : 1;
    }

    std::vector<TestCase> tests;
    bool currentFailed = false;
    std::vector<TestFailure> currentFailures;

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
        currentFailures.push_back({"<exception>", 0, "", std::string(what)});
    }

    static void printHeader(std::size_t count)
    {
        std::cout << "NanoTest — running " << count << " test(s)\n\n";
    }

    static void printPass(std::string_view name)
    {
        std::cout << "  PASS  " << name << '\n';
    }

    void printFailure(std::string_view name)
    {
        std::cout << "  FAIL  " << name << '\n';
        for (auto& f: currentFailures)
        {
            if (f.line > 0)
                std::cout << "        " << f.file << ':' << f.line
                          << ": " << f.expression << '\n';
            else if (!f.message.empty())
                std::cout << "        exception: " << f.message << '\n';
        }
    }

    static void printSummary(int passed, int failed)
    {
        std::cout << '\n'
                  << passed << " passed, " << failed << " failed, "
                  << passed + failed << " total\n";
    }
};

bool test(std::string name, const std::function<void()>& body)
{
    Registry::instance().add(std::move(name), body);
    return true;
}

int run(int argc, char* argv[])
{
    auto& reg = Registry::instance();

    for (auto i = 1; i < argc; ++i)
    {
        auto arg = std::string_view(argv[i]);
        if (arg == "--list-tests")
        {
            reg.listTests();
            return 0;
        }
        if (arg == "--test" && i + 1 < argc)
            return reg.run(argv[++i]);
    }

    return reg.run();
}

void check(bool expr, std::string_view exprStr, const std::source_location& loc)
{
    if (!expr)
        Registry::instance().fail(loc, exprStr);
}

} // namespace nano
