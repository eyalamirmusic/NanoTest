#include "NanoTest.h"

#include <format>
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

    int run()
    {
        auto passed = 0;
        auto failed = 0;

        std::cout << std::format("NanoTest — running {} test(s)\n\n", tests.size());

        for (auto& [name, body]: tests)
        {
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
                std::cout << std::format("  FAIL  {}\n", name);
                for (auto& f: currentFailures)
                {
                    if (f.line > 0)
                    {
                        std::cout << std::format(
                            "        {}:{}: {}\n", f.file, f.line, f.expression);
                    }
                    else if (!f.message.empty())
                    {
                        std::cout
                            << std::format("        exception: {}\n", f.message);
                    }
                }
            }
            else
            {
                ++passed;
                std::cout << std::format("  PASS  {}\n", name);
            }
        }

        std::cout << std::format(
            "\n{} passed, {} failed, {} total\n", passed, failed, passed + failed);

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

int run()
{
    return Registry::instance().run();
}

void check(bool expr, std::string_view exprStr, const std::source_location& loc)
{
    if (!expr)
        Registry::instance().fail(loc, exprStr);
}

} // namespace nano
