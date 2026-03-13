# NanoTest

A minimal C++20/CMake testing library.
One header, one assertion, automatic CTest integration.

Macro-free by design.

## Quick Start

**tests.cpp:**

```cpp
#include <NanoTest/NanoTest.h>

using namespace nano;

auto addition = test("Addition") = []
{
    check(2 + 2 == 4);
};
```

Using `using namespace nano;` keeps test files expressive and free of repetitive prefixes.

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
nano_add_executable(MyTests SOURCES tests.cpp)
```

`nano_add_executable` creates a test executable with a generated `main()`, links NanoTest, and registers each test case individually with CTest. You can also link your own libraries using `TARGETS`:

```cmake
nano_add_executable(MyTests SOURCES tests.cpp TARGETS MyLib)
```

### Custom `main()`

By default, `nano_add_executable` provides a `main()` that calls `nano::run`. If you need custom setup before running tests, pass `NO_MAIN` and write your own:

```cmake
nano_add_executable(MyTests SOURCES tests.cpp NO_MAIN)
```

```cpp
int main(int argc, char* argv[])
{
    // custom setup ...
    return nano::run(argc, argv);
}
```

### Setting up an existing target

Use `nano_setup_target` to set up a target you've already created with `add_executable`. This links NanoTest (without a generated `main()`) and discovers tests:

```cmake
add_executable(MyTests tests.cpp)
nano_setup_target(MyTests TARGETS MyLib)
```

Pass `WITH_MAIN` if you want the generated `main()`:

```cmake
add_executable(MyTests tests.cpp)
nano_setup_target(MyTests WITH_MAIN)
```

Both functions also accept `TEST_PREFIX` to namespace CTest entries when test names might collide across targets:

```cmake
nano_add_executable(MyTests SOURCES tests.cpp TEST_PREFIX "MyTests:")
```

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
| `nano::test(name) = body` | Register a test case |
| `nano::check(expr)` | Assert that an expression is true |
| `nano::run(argc, argv)` | Run tests, supports `--list-tests` and `--test <name>` |

## Requirements

- C++20
- CMake 3.21+

## License

[MIT](LICENSE.txt)
