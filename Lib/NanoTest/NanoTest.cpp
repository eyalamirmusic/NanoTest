#include "NanoTest.h"

#include "Registry.h"

namespace nano
{

TestProxy& TestProxy::operator=(const std::function<void()>& body)
{
    test(name, body);
    return *this;
}

bool test(std::string_view name, const std::function<void()>& body)
{
    getRegistry().add(name, body);
    return true;
}

TestProxy test(std::string_view name)
{
    return {name};
}

int run(const RunOptions& options)
{
    auto& reg = getRegistry();

    if (options.listTests)
    {
        reg.listTests();
        return 0;
    }

    return reg.run(options.test);
}

int run(int argc, char* argv[])
{
    auto options = RunOptions();

    for (auto i = 1; i < argc; ++i)
    {
        auto arg = std::string_view(argv[i]);

        if (arg == "--list-tests")
            options.listTests = true;
        else if (arg == "--test")
            options.test = arg;
    }

    return run(options);
}

void check(bool expr, std::string_view exprStr, const std::source_location& loc)
{
    if (!expr)
        getRegistry().fail(loc, exprStr);
}

} // namespace nano
