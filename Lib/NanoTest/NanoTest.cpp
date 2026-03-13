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
        auto passed = 0;
        auto failed = 0;
        auto total = 0;

        for (auto& [name, body]: tests)
        {
            if (!filter.empty() && name != filter)
                continue;
            ++total;
        }

        std::cout << "NanoTest — running " << total << " test(s)\n\n";

        for (auto& [name, body]: tests)
        {
            if (!filter.empty() && name != filter)
                continue;

            currentFailed = false;
            currentFailures.clear();

            try
            {
                body();
            }
            catch (const std::exception& e)
            {
                currentFailed = true;
                currentFailures.push_back({"<exception>", 0, "", e.what()});
            }
            catch (...)
            {
                currentFailed = true;
                currentFailures.push_back(
                    {"<exception>", 0, "", "unknown exception"});
            }

            if (currentFailed)
            {
                ++failed;
                std::cout << "  FAIL  " << name << '\n';
                for (auto& f: currentFailures)
                {
                    if (f.line > 0)
                    {
                        std::cout << "        " << f.file << ':' << f.line
                                  << ": " << f.expression << '\n';
                    }
                    else if (!f.message.empty())
                    {
                        std::cout << "        exception: " << f.message << '\n';
                    }
                }
            }
            else
            {
                ++passed;
                std::cout << "  PASS  " << name << '\n';
            }
        }

        std::cout << '\n'
                  << passed << " passed, " << failed << " failed, "
                  << passed + failed << " total\n";

        return failed == 0 ? 0 : 1;
    }

    std::vector<TestCase> tests;
    bool currentFailed = false;
    std::vector<TestFailure> currentFailures;
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
