#pragma once

#include "Types.h"

#include <iostream>
#include <string_view>
#include <vector>

namespace nano
{

struct Printer
{
    static void header(std::size_t count)
    {
        std::cout << "NanoTest — running " << count << " test(s)\n\n";
    }

    static void pass(std::string_view name)
    {
        std::cout << "  PASS  " << name << '\n';
    }

    static void failure(std::string_view name,
                        const std::vector<TestFailure>& failures)
    {
        std::cout << "  FAIL  " << name << '\n';
        for (auto& f: failures)
        {
            if (f.line > 0)
                std::cout << "        " << f.file << ':' << f.line
                          << ": " << f.expression << '\n';
            else if (!f.message.empty())
                std::cout << "        exception: " << f.message << '\n';
        }
    }

    static void summary(int passed, int failed)
    {
        std::cout << '\n'
                  << passed << " passed, " << failed << " failed, "
                  << passed + failed << " total\n";
    }
};

} // namespace nano
