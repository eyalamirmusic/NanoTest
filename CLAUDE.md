# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is NanoTest

A minimal C++20 testing library inspired by Catch2/GTest. It provides two equivalent APIs: macro-based (`NANO_TEST`/`NANO_ASSERT`) and function-based (`nano::test`/`nano::check`). Individual tests are auto-discovered by CTest via a post-build step that invokes the executable with `--list-tests`.

## Build & Test Commands

```bash
# Configure and build (macOS universal)
cmake -B build -DCMAKE_CXX_STANDARD=20 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13 -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build

# Run all tests via CTest
ctest --test-dir build --output-on-failure

# Run a single test by name
./build/Tests/NanoTestBasic --test TrueIsTrue

# List all tests in an executable
./build/Tests/NanoTestBasic --list-tests

# iOS (cross-compile only, no test execution)
cmake -B build-ios -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=14.0 -G Xcode
cmake --build build-ios -- -sdk iphonesimulator

# Format code
clang-format -i Lib/NanoTest/*.h Lib/NanoTest/*.cpp Tests/*.cpp
```

## Architecture

The public API is a single header (`Lib/NanoTest/NanoTest.h`) exposing only three free functions (`nano::test`, `nano::run`, `nano::check`) and three macros. All internal types are hidden in implementation-only headers inside `Lib/NanoTest/`:

- **Types.h** — `TestFailure` and `TestCase` structs
- **Printer.h** — all console output (header, pass, failure, summary)
- **Registry.h** — singleton test registry, filtering, execution, exception handling
- **NanoTest.cpp** — wires the public API to the Registry; parses `--list-tests` and `--test` CLI args

**CMake integration** (`Lib/NanoTestDiscoverTests.cmake` + `NanoTestAddTests.cmake`): `nano_add_executable(MyTests file.cpp)` creates a test target, links NanoTest, and sets up post-build test discovery. When cross-compiling, discovery is skipped and the target is registered as a single CTest entry.

## Code Style

- C++20, Allman braces, 4-space indent, 85-column limit (see `.clang-format`)
- Use `auto` whenever possible for variable declarations
- No underscore suffixes on class members (use `tests` not `tests_`; use `nameToUse` for parameters that would shadow)
- Class member variables go at the end of the class definition
- Avoid `std::format` — it requires macOS 13.3+ due to `to_chars`; use `std::cout <<` instead
