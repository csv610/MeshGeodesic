# MeshGeodesic

A modernized C++20 port of the original Geodesic library for computing exact and approximate shortest paths on triangular meshes.

## Overview

This project is a modernized port of the geodesic library originally developed by **Danil Kirsanov**. It provides efficient implementations of several geodesic algorithms, including:

*   **Exact Geodesic Algorithm**: The Mitchell-Mount-Papadimitriou (MMP) algorithm for exact shortest paths.
*   **Dijkstra's Algorithm**: Fast vertex-to-vertex shortest paths.
*   **Subdivision Algorithm**: An approximate algorithm with adjustable precision.

## Contributions & Modernization

This fork introduces several significant improvements to the original codebase to align it with modern C++ standards and development practices:

1.  **C++20 Standardization**: The entire codebase has been updated and verified to compile under the C++20 standard.
2.  **Modern Memory Management**: Replaced all instances of the deprecated `std::auto_ptr` with `std::unique_ptr`, ensuring better memory safety and compatibility with modern compilers.
3.  **Safety & Robustness**:
    *   Replaced unsafe `memcpy` operations on non-trivially copyable types with standard C++ object assignments, preventing potential memory corruption and object slicing.
    *   Fixed ISO C++ warnings regarding the conversion of string literals to non-const `char*`.
    *   Added `assert` and return safety to control paths in API functions.
4.  **Modern Build System**: Added a comprehensive `CMakeLists.txt` for easy cross-platform compilation and dependency management.
5.  **Project Organization**: Restructured the project for better clarity, moving example applications to the `app/` directory and separating headers/sources in `src/`.
6.  **Unit Testing Suite**: Implemented a new automated testing suite using the `doctest` framework, covering mesh construction and geodesic distance verification.
7.  **Benchmarking Tool**: Added a dedicated benchmarking utility to compare the performance and accuracy of different geodesic algorithms on realistic mesh data.

## Requirements

*   A C++20 compatible compiler (e.g., GCC 10+, Clang 10+, MSVC 2019+)
*   CMake 3.10 or higher
*   Boost (Optional, required only for the MATLAB API)

## Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

### Running Tests
After building, you can run the unit tests:
```bash
./test_geodesic
```

### Running Benchmarks
To compare algorithm performance:
```bash
./benchmark_geodesic
```

## Original Credits

The original code was developed by **Danil Kirsanov** (2008) and was released under the MIT License.

## License

This project is licensed under the MIT License - see the original headers for details.
