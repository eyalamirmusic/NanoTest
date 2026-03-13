# NanoTest

A minimal C++20/CMake testing library.
One header, one assertion, automatic CTest integration.

It's also macro-free (but macros exist if you want them)

## Quick Start

**tests.cpp:**

```cpp
#include <NanoTest/NanoTest.h>

auto addition = nano::test("Addition", []
{
    nano::check(2 + 2 == 4);
});

int main(int argc, char* argv[])
{
    return nano::run(argc, argv);
}
```

**CMakeLists.txt:**

```cmake
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

Build and run:

```bash
cmake -B build && cmake --build build && ctest --test-dir build
```

```
1/1 Test #1: Addition .....   Passed    0.00 sec
```

That's it. Each test is automatically discovered and registered with CTest.

## Macro API (Optional)

A separate header provides macros that add expression stringification to failure messages:

```cpp
#include <NanoTest/NanoTestMacros.h>

NANO_TEST(Addition)
{
    NANO_ASSERT(2 + 2 == 4);
}

NANO_TEST_MAIN
```

Both styles can be mixed in the same file.

## CMake Details

`nano_add_executable` creates a test target, links NanoTest, and registers each test case individually with CTest. It is equivalent to:

```cmake
add_executable(MyTests tests.cpp)
target_link_libraries(MyTests PRIVATE NanoTest)
nano_discover_tests(MyTests)
```
You can and should of course link the test app with your libraries

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

| Function / Macro | Description |
|---|---|
| `nano::test(name, body)` | Register a test case |
| `nano::check(expr)` | Assert that an expression is true |
| `nano::run(argc, argv)` | Run tests, supports `--list-tests` and `--test <name>` |
| `NANO_TEST(name)` | Define and register a test case |
| `NANO_ASSERT(expr)` | Assert with expression stringification |
| `NANO_TEST_MAIN` | Generate a `main()` that forwards to `nano::run` |

## Requirements

- C++20
- CMake 3.21+

## License

[MIT](LICENSE.txt)
