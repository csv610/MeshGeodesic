# Technical FAQ: Hidden Treasures of MeshGeodesic

This document explores the advanced engineering and algorithmic depths of the MeshGeodesic library, highlighting features that often go unnoticed but are critical for high-performance geometric computing.

---

### 1. What makes the MMP implementation in this library "rare"?
The Mitchell-Mount-Papadimitriou (MMP) algorithm is notoriously difficult to implement due to the complexity of managing "windows" of geodesic distances that propagate across edges. While many libraries use approximate Dijkstra-based approaches, MeshGeodesic provides a mathematically exact implementation that correctly handles "saddle" vertices by allowing paths to pass through them, a feature many modern implementations skip or approximate.

### 2. How does the `SimpleMemoryAllocator` achieve zero-fragmentation performance?
The library uses a specialized arena-style allocator (`SimpleMemoryAllocator`). Instead of making thousands of small `new`/`delete` calls for `Interval` objects (which would destroy the CPU cache), it pre-allocates large contiguous blocks of memory. Deallocation is handled by simply resetting the block pointer, ensuring that the critical path of the window propagation remains entirely within the L1/L2 cache.

### 3. What is the "Window Propagation" logic in `geodesic_algorithm_exact.cpp`?
Unlike graph-based algorithms, the MMP algorithm propagates "windows"—linear functions representing the distance to a source. The library tracks these windows as they split, merge, and "clip" against each other on mesh edges. This edge-based propagation is what allows for sub-triangle precision, allowing a path to cross a single triangle at any arbitrary angle.

### 4. How does `SurfacePoint` bridge the gap between discrete and continuous geometry?
The `SurfacePoint` class is a polymorphic abstraction that can represent a point on a vertex, an edge, or in the middle of a face. This allows the library to compute distances between non-vertex locations—for example, from the exact center of a face to a specific point on an edge—without requiring the user to modify the underlying mesh topology.

### 5. Why was `std::size_t` mandated over `unsigned` during the refactor?
On 64-bit architectures, `unsigned int` is often limited to 4.2 billion elements. For high-resolution LIDAR scans or medical imaging meshes with hundreds of millions of triangles, `unsigned` risks silent overflow. Standardizing on `std::size_t` ensures that MeshGeodesic can handle meshes that consume the entire available system RAM without index-related crashes.

### 6. How are non-manifold and degenerate meshes handled?
The `Mesh::verify()` function performs a topological audit. The library is specifically designed to handle "boundary" edges (where a face is missing) and "saddle" vertices. By using `std::numeric_limits<double>::infinity()`, the algorithm correctly isolates disconnected components of a mesh, ensuring that propagation doesn't "leak" or crash when it hits a hole in the geometry.

### 7. What is the significance of the `IntervalList` per edge?
Every edge in the mesh maintains an `IntervalList` of all distance windows currently covering it. The library uses a sophisticated "intersection" logic to determine if a new window provides a shorter path than an existing one. If it does, the existing windows are clipped or replaced. This "competitive" propagation is the core of the library's exactness.

### 8. Can I use the library for Farthest Point Sampling (FPS)?
Yes. The library includes a specialized implementation of FPS in `app/farthest_point_sampling.cpp`. By leveraging the `best_source()` API, it can compute a set of points that are maximally distant from each other, which is a foundational technique for remeshing, surface reconstruction, and point cloud simplification.

### 9. How does the library handle "Saddle Vertices"?
A vertex is a "saddle" if the sum of the angles of its incident triangles is greater than 2π. The MMP algorithm treats these as "pseudo-sources." When a geodesic hit such a vertex, it behaves like a new source for the remaining distance. MeshGeodesic identifies these during `build_adjacencies()` and marks them using `v.saddle_or_boundary()`, allowing the propagation engine to branch paths correctly.

### 10. How can I benchmark my own algorithm against MeshGeodesic?
The `tests/benchmark_geodesic.cpp` uses the `doctest` framework to provide precise timing for different algorithms (Exact, Dijkstra, Subdivision). Because the library is now modularized into a static `geodesic` library, researchers can simply link their own methods and use the `Mesh` data structure to perform fair, "apples-to-apples" comparisons of execution time and path error.
