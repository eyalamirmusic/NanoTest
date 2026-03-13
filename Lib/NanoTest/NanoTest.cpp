#include "NanoTest.h"

#include <format>
#include <iostream>

namespace nano
{

Registry& Registry::instance()
{
    static auto reg = Registry();
    return reg;
}

void Registry::add(std::string nameToUse, std::function<void()> bodyToUse)
{
    tests.push_back({std::move(nameToUse), std::move(bodyToUse)});
}

void Registry::fail(const std::source_location& loc,
                    std::string_view expression,
                    std::string_view message)
{
    currentFailed = true;
    currentFailures.push_back({loc.file_name(),
                               static_cast<int>(loc.line()),
                               std::string(expression),
                               std::string(message)});
}

AutoRegister::AutoRegister(std::string nameToUse, const Callback& bodyToUse)
{
    Registry::instance().add(std::move(nameToUse), std::move(bodyToUse));
}

bool test(std::string name, const Callback& body)
{
    Registry::instance().add(std::move(name), std::move(body));
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

int Registry::run()
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
            currentFailures.push_back({"<exception>", 0, "", "unknown exception"});
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
                    std::cout << std::format("        exception: {}\n", f.message);
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

} // namespace nano
