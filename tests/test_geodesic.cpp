#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "geodesic_mesh.h"
#include "geodesic_algorithm_exact.h"
#include "geodesic_algorithm_dijkstra.h"

TEST_CASE("Basic Mesh Construction") {
    std::vector<double> points = {
        0, 0, 0,
        1, 0, 0,
        0, 1, 0,
        1, 1, 0
    };
    std::vector<unsigned> faces = {
        0, 1, 2,
        1, 3, 2
    };

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);

    CHECK(mesh.vertices().size() == 4);
    CHECK(mesh.faces().size() == 2);
    CHECK(mesh.edges().size() == 5); // 4 boundary + 1 internal
}

TEST_CASE("Geodesic Distance on Flat Mesh") {
    // Two triangles forming a square [0,1]x[0,1]
    std::vector<double> points = {
        0, 0, 0, // 0
        1, 0, 0, // 1
        0, 1, 0, // 2
        1, 1, 0  // 3
    };
    std::vector<unsigned> faces = {
        0, 1, 2,
        1, 3, 2
    };

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);
    geodesic::GeodesicAlgorithmExact algorithm(&mesh);

    // Source at (0,0,0)
    geodesic::SurfacePoint source(&mesh.vertices()[0]);
    std::vector<geodesic::SurfacePoint> all_sources(1, source);
    algorithm.propagate(all_sources);

    // Distance to (1,1,0) should be sqrt(2)
    geodesic::SurfacePoint target(&mesh.vertices()[3]);
    double distance;
    algorithm.best_source(target, distance);

    CHECK(distance == doctest::Approx(sqrt(2.0)));

    // Distance to (1,0,0) should be 1.0
    geodesic::SurfacePoint target1(&mesh.vertices()[1]);
    algorithm.best_source(target1, distance);
    CHECK(distance == doctest::Approx(1.0));
}

TEST_CASE("Geodesic Path on 3D Mesh (Cube Corner)") {
    // A simple L-shape in 3D to force a path that isn't a straight line in 3D space but is on the surface
    std::vector<double> points = {
        0, 0, 0, // 0: Origin
        1, 0, 0, // 1: X-axis
        0, 1, 0, // 2: Y-axis
        0, 0, 1  // 3: Z-axis
    };
    // Triangle in XY plane (0,1,2) and Triangle in XZ plane (0,1,3)
    std::vector<unsigned> faces = {
        0, 1, 2,
        0, 3, 1
    };

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);
    geodesic::GeodesicAlgorithmExact algorithm(&mesh);

    // Source at (0,1,0)
    geodesic::SurfacePoint source(&mesh.vertices()[2]);
    std::vector<geodesic::SurfacePoint> all_sources(1, source);
    algorithm.propagate(all_sources);

    // Target at (0,0,1)
    // Shortest path on surface: (0,1,0) -> (origin or somewhere on edge 0-1) -> (0,0,1)
    // Actually, on these two triangles, the shortest path from (0,1,0) to (0,0,1) 
    // passes through the edge (0,1).
    // If we unfold the triangles [2,0,1] and [3,0,1] into a plane:
    // 2 is at (0,1), 0 is at (0,0), 1 is at (1,0), 3 is at (0,-1) (after rotation around 0-1)
    // Distance from (0,1) to (0,-1) is 2.0.
    
    geodesic::SurfacePoint target(&mesh.vertices()[3]);
    double distance;
    algorithm.best_source(target, distance);

    CHECK(distance == doctest::Approx(2.0));
}

TEST_CASE("Dijkstra Comparison") {
    std::vector<double> points = {
        0, 0, 0,
        1, 0, 0,
        0, 1, 0,
        1, 1, 0
    };
    std::vector<unsigned> faces = {
        0, 1, 2,
        1, 3, 2
    };

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);
    
    geodesic::GeodesicAlgorithmExact exact(&mesh);
    geodesic::GeodesicAlgorithmDijkstra dijkstra(&mesh);

    geodesic::SurfacePoint source(&mesh.vertices()[0]);
    std::vector<geodesic::SurfacePoint> all_sources(1, source);
    
    exact.propagate(all_sources);
    dijkstra.propagate(all_sources);

    geodesic::SurfacePoint target(&mesh.vertices()[3]);
    double d_exact, d_dijkstra;
    exact.best_source(target, d_exact);
    dijkstra.best_source(target, d_dijkstra);

    // Dijkstra on vertices only should give 1.0 + 1.0 = 2.0 (0->1->3 or 0->2->3)
    // because it only moves along edges.
    // Exact should give sqrt(2)
    CHECK(d_exact == doctest::Approx(sqrt(2.0)));
    CHECK(d_dijkstra == doctest::Approx(2.0));
}
