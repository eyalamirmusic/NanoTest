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

inline void summary(int passed, int failed)
{
    std::cout << '\n'
              << passed << " passed, " << failed << " failed, " << passed + failed
              << " total\n";
}
}; // namespace Printer

} // namespace nano
