# NanoTest

A minimal C++20/CMake testing library.
One header, one assertion, automatic CTest integration.

Macro-free by design.

## Quick Start

**tests.cpp:**

```cpp
#include <NanoTest/NanoTest.h>

using namespace nano;

auto addition = test("Addition", []
{
    check(2 + 2 == 4);
});
```

Using `using namespace nano;` keeps test files expressive and free of repetitive prefixes.

## Providing `main()`

NanoTest needs a `main()` function that calls `nano::run`. You can either link the built-in one or write your own.

**Using `NanoTestMain` (recommended)** — link the provided `main()` so your test files don't need one:

```cmake
add_executable(MyTests tests.cpp)
target_link_libraries(MyTests PRIVATE NanoTestMain)
nano_discover_tests(MyTests)
```

**Writing your own** — useful if you need custom setup before running tests:

```cpp
int main(int argc, char* argv[])
{
    return nano::run(argc, argv);
}
```

## CMake Integration

**CMakeLists.txt:**

```cmake
cmake_minimum_required(VERSION 3.21)
project(NanoTestExample)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        NanoTest
        GIT_REPOSITORY https://github.com/eyalamirmusic/NanoTest.git
        GIT_TAG main
)

FetchContent_MakeAvailable(NanoTest)

enable_testing()
nano_add_executable(MyTests tests.cpp)
```

`nano_add_executable` creates a test target, links NanoTest, and registers each test case individually with CTest. It is equivalent to:

```cmake
add_executable(MyTests tests.cpp)
target_link_libraries(MyTests PRIVATE NanoTest)
nano_discover_tests(MyTests)
```

You can and should of course link the test app with your libraries.

Build and run:

```bash
cmake -B build && cmake --build build && ctest --test-dir build
```

```
1/1 Test #1: Addition .....   Passed    0.00 sec
```

Each test is automatically discovered and registered with CTest.

### Running Tests

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run a single test by name
./build/MyTests --test Addition

# List all registered tests
./build/MyTests --list-tests
```

## Cross-Compilation

NanoTest builds on iOS, Emscripten, and other cross-compilation targets. Test discovery is automatically skipped when cross-compiling, falling back to a single CTest entry per executable.

## API Reference

| Function | Description |
|---|---|
| `nano::test(name, body)` | Register a test case |
| `nano::check(expr)` | Assert that an expression is true |
| `nano::run(argc, argv)` | Run tests, supports `--list-tests` and `--test <name>` |

## Requirements

- C++20
- CMake 3.21+

## License

[MIT](LICENSE.txt)
