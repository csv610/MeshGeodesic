# MeshGeodesic

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A modernized, high-performance C++20 port of the original Geodesic library for computing exact and approximate shortest paths on triangular meshes.

## 🚀 Overview

**MeshGeodesic** provides efficient implementations of geodesic algorithms on discrete surfaces. It is based on the classic work of **Danil Kirsanov** and has been completely overhauled for modern C++ development.

### Included Algorithms:
*   **Exact Geodesic (MMP)**: Computes mathematically exact shortest paths using the Mitchell-Mount-Papadimitriou algorithm.
*   **Dijkstra's Algorithm**: Rapid vertex-to-vertex shortest path estimation on the mesh graph.
*   **Subdivision Algorithm**: A flexible approximation that balances speed and precision by virtually subdividing mesh edges.

## ✨ Modernization Highlights

This fork transforms the original 2008 codebase into a production-ready modern library:

*   **C++20 Standards**: Built from the ground up to support modern compilers and language features.
*   **Memory Safety**: Replaced legacy `std::auto_ptr` with `std::unique_ptr` and eliminated unsafe memory operations.
*   **Robust Architecture**: Surgical fixes for ISO C++ compliance, improved type safety, and better error handling.
*   **Developer Friendly**: Modern CMake build system, comprehensive unit tests (`doctest`), and integrated benchmarking.

## 🛠 Quick Start

### Installation

```bash
git clone https://github.com/your-repo/MeshGeodesic.git
cd MeshGeodesic
mkdir build && cd build
cmake ..
make -j
```

### Basic Usage Example

```cpp
#include "geodesic_algorithm_exact.h"
#include "geodesic_mesh.h"

int main() {
    std::vector<double> points;
    std::vector<unsigned> faces;

    // Load your mesh data
    geodesic::read_mesh_from_file("mesh.txt", points, faces);

    // Initialize the mesh
    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);

    // Initialize the algorithm
    geodesic::GeodesicAlgorithmExact algorithm(&mesh);

    // Define source and target points
    geodesic::SurfacePoint source(&mesh.vertices()[0]);
    geodesic::SurfacePoint target(&mesh.vertices()[100]);

    // Compute the shortest path
    std::vector<geodesic::SurfacePoint> path;
    algorithm.compute_geodesic(source, target, path);

    // Calculate and print path length
    double path_length = geodesic::length(path);
    std::cout << "Path length: " << path_length << std::endl;
    return 0;
}
```

## 📂 Data Format

The library includes a simple text-based mesh loader. The expected format for `.txt` mesh files is:

```text
# Number of vertices and faces
5 4

# Vertices: x y z
0.0 0.0 0.0
1.0 0.0 0.0
1.0 1.0 0.0
0.0 1.0 0.0
0.5 0.5 1.0

# Faces: v1 v2 v3 (0-indexed)
0 1 4
1 2 4
2 3 4
3 0 4
```

## 🧪 Testing & Benchmarking

We maintain high code quality through rigorous testing:

*   **Run Unit Tests**: `./test_geodesic`
*   **Run Benchmarks**: `./benchmark_geodesic`

Detailed documentation on algorithms and API usage can be found in the [User Guide](docs/UserGuide.md).

## 📜 Credits & License

*   **Original Author**: Danil Kirsanov (2008)
*   **Modern Port**: Managed by the MeshGeodesic contributors.
*   **License**: This project is licensed under the **MIT License**. See the `LICENSE` file or source headers for details.
