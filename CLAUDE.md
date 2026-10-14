# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is NanoTest

A minimal, macro-free C++20 testing library inspired by Catch2/GTest. The API consists of three free functions: `nano::test`, `nano::check`, and `nano::run`. Individual tests are auto-discovered by CTest via a post-build step that invokes the executable with `--list-tests`.

## Build & Test Commands

```bash
# Configure and build (macOS universal)
cmake -B build -DCMAKE_CXX_STANDARD=20 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build

# Run all tests via CTest
ctest --test-dir build --output-on-failure

# Run a single test by name
./build/Tests/NanoTestFunctions --test CheckTrue

# Run a group: the test named Net and every test under Net/
./build/Tests/NanoTestFunctions --test-group Net

# List all tests in an executable
./build/Tests/NanoTestFunctions --list-tests

# iOS (cross-compile only, no test execution)
cmake -B build-ios -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 -G Xcode
cmake --build build-ios -- -sdk iphonesimulator

# Format code
clang-format -i Lib/NanoTest/*.h Lib/NanoTest/*.cpp Tests/*.cpp
```

## Architecture

The public API is a single header (`Lib/NanoTest/NanoTest.h`) exposing three free functions (`nano::test`, `nano::run`, `nano::check`). A separate `NanoTestMain` library provides a prebuilt `main()` so test files don't need one. All internal types are hidden in implementation-only headers inside `Lib/NanoTest/`:

- **Types.h** — `TestFailure` and `TestCase` structs
- **Printer.h** — all console output (header, pass, failure, summary)
- **Registry.h** — singleton test registry, filtering, execution, exception handling
- **NanoTest.cpp** — wires the public API to the Registry; parses `--list-tests`, `--test` and `--test-group` CLI args

**CMake integration** (`Lib/NanoTestDiscoverTests.cmake` + `NanoTestAddTests.cmake`): `nano_add_executable(MyTests file.cpp)` creates a test target, links NanoTest, and sets up post-build test discovery. `GROUP_BY CASE|PREFIX|EXECUTABLE` (default `CASE`, or `NANO_TEST_GROUP_BY`) decides whether discovery registers one CTest entry per case, per first name segment (`--test-group`), or per executable; `PROPERTIES` applies through a configure-time include file over the CTest-time `<TARGET>_TESTS` list. The grouping mode is on the POST_BUILD command line, so changing it relinks and rediscovers. When cross-compiling, discovery is skipped and the target is registered as a single CTest entry.

`Tests/CheckDiscovery.cmake` checks the registered entries through `ctest --show-only=json-v1`, reruns `NanoTestAddTests.cmake` in each grouping mode to check it prints no warning, and runs `FailingGroup` to check how a grouped failure is reported.

## Code Style

- C++20, Allman braces, 4-space indent, 85-column limit (see `.clang-format`)
- Use `auto` whenever possible for variable declarations
- No underscore suffixes on class members (use `tests` not `tests_`; use `nameToUse` for parameters that would shadow)
- Class member variables go at the end of the class definition
- Avoid `std::format` — it requires macOS 13.3+ due to `to_chars`; use `std::cout <<` instead
