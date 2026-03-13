#include "NanoTest.h"

#include "Registry.h"

namespace nano
{

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
