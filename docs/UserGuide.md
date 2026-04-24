# Geodesic Library User Guide: A Journey into Discrete Geometry

Welcome to the **Geodesic Library User Guide**. This guide is designed for undergraduate students and researchers who are interested in the fascinating world of discrete geometry, computer graphics, and geometry processing.

---

## Table of Contents
1.  [Preface: The Shortest Path Problem](#preface-the-shortest-path-problem)
2.  [Chapter 1: Foundations of the Geodesic Library](#chapter-1-foundations-of-the-geodesic-library)
3.  [Chapter 2: Getting Started: Installation and Setup](#chapter-2-getting-started-installation-and-setup)
4.  [Chapter 3: The Mesh: Our Geometric Canvas](#chapter-3-the-mesh-our-geometric-canvas)
5.  [Chapter 4: Navigating the Surface: Algorithms and Methods](#chapter-4-navigating-the-surface-algorithms-and-methods)
6.  [Chapter 5: Practical Applications: From Theory to Code](#chapter-5-practical-applications-from-theory-to-code)
7.  [Chapter 6: API Deep Dive: Class Structures and Interactions](#chapter-6-api-deep-dive-class-structures-and-interactions)
8.  [Epilogue: Looking Ahead](#epilogue-looking-ahead)

---

## Preface: The Shortest Path Problem

In Euclidean space, the shortest path between two points is a straight line. However, what if we are confined to a surface, such as the surface of the Earth, a human face, or a complex industrial part? On these surfaces, the "straight line" becomes a **geodesic**.

Finding geodesics on discrete surfaces—represented as triangle meshes—is a fundamental problem in computer science. It has applications in medical imaging, architectural design, robotics, and character animation. This library provides you with the tools to solve these problems efficiently and accurately.

---

## Chapter 1: Foundations of the Geodesic Library

The **MeshGeodesic** library is a modernized C++20 port of the classic geodesic library developed by Danil Kirsanov. It focuses on performance, clarity, and ease of use.

### Core Features:
- **Exact Geodesics**: Implementation of the Mitchell-Mount-Papadimitriou (MMP) algorithm.
- **Approximations**: Dijkstra's algorithm and the Subdivision algorithm for scenarios where speed is more critical than sub-millimeter precision.
- **Modern C++**: Leverages `std::unique_ptr`, C++20 standards, and a robust build system.

---

## Chapter 2: Getting Started: Installation and Setup

### Prerequisites
To use this library, you will need:
- A C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+).
- **CMake** (3.10 or higher).
- (Optional) **Boost** for certain advanced features like the MATLAB API.

### Building the Library
Building is straightforward using CMake:

```bash
mkdir build
cd build
cmake ..
make -j4
```

This will compile the library and the example applications located in the `app/` directory.

### Running Your First Test
Verify your installation by running the test suite:
```bash
./test_geodesic
```

---

## Chapter 3: The Mesh: Our Geometric Canvas

The primary data structure in this library is the `geodesic::Mesh`. A mesh consists of three main elements:
1.  **Vertices**: Points in 3D space $(x, y, z)$.
2.  **Edges**: Lines connecting two vertices.
3.  **Faces**: Triangles formed by three vertices and three edges.

### Loading a Mesh
The library provides a simple utility to load meshes from text files:

```cpp
std::vector<double> points;	
std::vector<unsigned> faces;
geodesic::read_mesh_from_file("path/to/mesh.txt", points, faces);

geodesic::Mesh mesh;
mesh.initialize_mesh_data(points, faces);
```

### Mesh Requirements
For the algorithms to function correctly, the mesh should be:
- **Manifold**: Every edge is shared by at most two faces.
- **Connected**: There is a path between any two vertices.
- **Non-degenerate**: No zero-area triangles or zero-length edges.

---

## Chapter 4: Navigating the Surface: Algorithms and Methods

The library offers three primary algorithms, each with its own trade-offs between speed and accuracy.

### 1. The Exact Algorithm (`GeodesicAlgorithmExact`)
This is the "Gold Standard." It computes the mathematically exact shortest path on the surface of the mesh.
- **Use case**: When accuracy is paramount.
- **Mechanism**: It propagates "windows" across edges to track all possible shortest paths.

### 2. Dijkstra's Algorithm (`GeodesicAlgorithmDijkstra`)
A classic algorithm that restricts paths to the edges of the mesh.
- **Use case**: Quick distance estimates between vertices.
- **Note**: It does not allow paths to cross the interior of faces.

### 3. Subdivision Algorithm (`GeodesicAlgorithmSubdivision`)
A middle ground between Dijkstra and Exact. It virtually subdivides each edge into smaller segments.
- **Use case**: High-performance approximations.
- **Parameter**: `subdivision_level`. Increasing this improves accuracy but slows down computation.

---

## Chapter 5: Practical Applications: From Theory to Code

Let's see how to compute the distance between two points on a mesh.

### Single Source to Single Target
```cpp
geodesic::GeodesicAlgorithmExact algorithm(&mesh);
geodesic::SurfacePoint source(&mesh.vertices()[0]); // Starting point
geodesic::SurfacePoint target(&mesh.vertices()[100]); // End point

std::vector<geodesic::SurfacePoint> path;
algorithm.geodesic(source, target, path); // Finds the path

// Calculate path length
double length = 0;
for(size_t i = 0; i < path.size() - 1; ++i) {
    length += path[i].distance(path[i+1]);
}
```

### Multiple Sources
You can compute the distance from a set of sources (e.g., finding the nearest fire station in a city):
```cpp
std::vector<geodesic::SurfacePoint> sources;
sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[0]));
sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[50]));

algorithm.propagate(sources); // Computes distances across the whole mesh

// Query distance to any point
geodesic::SurfacePoint p(&mesh.vertices()[200]);
double distance;
unsigned closest_source_index = algorithm.best_source(p, distance);
```

---

## Chapter 6: API Deep Dive: Class Structures and Interactions

### `geodesic::SurfacePoint`
This class represents any point on the surface of the mesh. It can be:
- Exactly on a **Vertex**.
- On an **Edge**.
- Inside a **Face**.

### `geodesic::GeodesicAlgorithmBase`
The base class for all algorithms. Key methods include:
- `propagate()`: Spreads the geodesic distance from sources.
- `trace_back()`: After propagation, reconstructs the path from a target back to a source.
- `best_source()`: Finds the distance to the nearest source.

---

## Epilogue: Looking Ahead

Now that you have mastered the basics of the Geodesic library, you can begin integrating it into your own projects. Whether you are building a tool for 3D measurement, texture mapping, or mesh simplification, the ability to compute accurate distances on surfaces is a powerful asset.

Happy Coding!
