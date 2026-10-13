#pragma once

#include "Types.h"

#include <iostream>
#include <string_view>
#include <vector>

namespace nano
{

namespace Printer
{
inline void header(std::size_t count)
{
    std::cout << "NanoTest — running " << count << " test(s)\n\n";
}

inline void pass(std::string_view name)
{
    std::cout << "  PASS  " << name << '\n';
}

inline void failure(std::string_view name, const std::vector<TestFailure>& failures)
{
    std::cout << "  FAIL  " << name << '\n';

    for (auto& f: failures)
    {
        if (f.line > 0)
            std::cout << "        " << f.file << ':' << f.line << ": "
                      << f.expression << '\n';
        else if (!f.message.empty())
            std::cout << "        exception: " << f.message << '\n';
    }
}

inline void summary(int passed, const std::vector<std::string_view>& failedNames)
{
    auto failed = static_cast<int>(failedNames.size());

    std::cout << '\n'
              << passed << " passed, " << failed << " failed, " << passed + failed
              << " total\n";

    if (failedNames.empty())
        return;

    std::cout << "\nFailed tests:\n";

    for (auto name: failedNames)
        std::cout << "  " << name << '\n';
}
}; // namespace Printer

} // namespace nano
