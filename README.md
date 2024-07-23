# Left-leaning Red-black tree

## Overview

This repository contains two unit-tested C++ container class implemented based on a generic _left-leaning red-black tree_ (LLRB). This data structure was initially designed and implemented by <a href="https://sedgewick.io"/>Robert Sedgewick</a> @Department of Computer Science, Princeton University. The original paper can be found [here](https://sedgewick.io/wp-content/themes/sedgewick/papers/2008LLRB.pdf).

Traditional symbol table operations like search, insertion, deletion, order statistics are supported by `Map`. $\Theta(\lg N)$ worst-case runtime is guaranteed for search, insertion, and deletion. `Set` supports the equivalent operations for an ordered container but without symbol table lookups.

> [!WARNING]
> This implementation of the left-leaning red-black tree is intended for educational learning and experimentation purposes. While efforts have been made to ensure correctness and performance, it may not be optimized for all production scenarios.

## Supported Features

**TL;DR**: support most operations included in `std::map` and `std::set` with a few useful extras.

- Generic typing: generic typing of keys, values, and custom comparators are supported.
- Balanced Structure: Ensures logarithmic depth, maintaining efficient search, insertion, and removal of keys.
- Order Statistics: Supports operations like `rank`, `min`, `max`, `floor`, `ceiling`, and `rankSelect`.
- Iterator Support: Provides iterators for in-order traversal of key-value pairs.
- Serialization: Converts the tree into a string format using custom serialization functions.
- Memory Management: Includes a destructor for safe memory cleanup.

## Usage

### Prerequisites

To use and develop with these modules, ensure that your environment meets the following requirements.

- Basic usage: `C++ 14` compatible compiler (e.g., GCC 5.0 or later, Clang 3.4 or later, MSVC 2015 or later).
- Compile unit tests: [CMake](https://cmake.org/download/) version 3.20 or later is required to configure and build the unit tests.
- Run automated scripts: [Taskfile](https://taskfile.dev/installation/) version 3 is required to run the automation scripts.

Make sure these tools and versions are properly installed and configured in your development environment to fully utilize the features and tests provided by this project.

### Include Headers

To use these classes in your project:

1. Dependencies: ensure the header file `.hpp` and the implementation `.ipp`, and [deque.hpp](src/deque.hpp) are present and under the same directory;
2. Include API Header: include the header by `#include "map.hpp"` for example;
3. Adjust your build tool of choice if needed: refer to [CMakeLists.txt](CMakeLists.txt) for an example.

### API Manual

A list of all public functions are provided in [MANUAL.md](MANUAL.md).

### Run Unit Tests

Assuming the system has all the [prerequisites](#Prerequisites) for running the unit tests. Navigate to the current directory (containing README.md) and run the shell command:

```
task
```

which automatically compiles and run all the unit tests. More details can be found in [taskfile.yml](taskfile.yml).

### Caveats with Template Classes

**TL;DR**: the header provided cannot be compiled into a binary library file.

This container is implemented as a template class. Templates themselves do not produce executable code directly; instead, they serve as blueprints for generating code. This is also why the implementations are included in [map.ipp](src/map.ipp) instead of a regular source code file.

To generate actual executable code, templates need to be instantiated with specific types. The compiler uses these types to fill in the template, producing concrete code. For this process to work, the compiler must have access to the full template definition, including the instantiation.

## License

Distributed under the MIT License. See [LICENSE.txt](LICENSE.txt) for more information.
