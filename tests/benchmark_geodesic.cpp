#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

#include "geodesic_mesh.h"
#include "geodesic_algorithm_exact.h"
#include "geodesic_algorithm_dijkstra.h"
#include "geodesic_algorithm_subdivision.h"

using namespace geodesic;

void run_benchmark(const std::string& mesh_file) {
    std::vector<double> points;
    std::vector<unsigned> faces;
    if (!read_mesh_from_file(mesh_file.c_str(), points, faces)) {
        std::cerr << "Failed to read mesh: " << mesh_file << std::endl;
        return;
    }

    Mesh mesh;
    mesh.initialize_mesh_data(points, faces);

    std::cout << "Mesh: " << mesh_file << std::endl;
    std::cout << "Vertices: " << mesh.vertices().size() << ", Faces: " << mesh.faces().size() << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::left << std::setw(25) << "Algorithm" << std::setw(20) << "Time (ms)" << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    // Set up sources (10 random-ish vertices)
    std::vector<SurfacePoint> sources;
    for (int i = 0; i < 10; ++i) {
        sources.push_back(SurfacePoint(&mesh.vertices()[(i * 137) % mesh.vertices().size()]));
    }

    auto benchmark = [&](const std::string& name, GeodesicAlgorithmBase& algo) {
        auto start = std::chrono::high_resolution_clock::now();
        algo.propagate(sources);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
        std::cout << std::left << std::setw(25) << name << std::setw(20) << duration << std::endl;
    };

    GeodesicAlgorithmExact exact(&mesh);
    benchmark("Exact", exact);

    GeodesicAlgorithmDijkstra dijkstra(&mesh);
    benchmark("Dijkstra", dijkstra);

    GeodesicAlgorithmSubdivision sub1(&mesh, 1);
    benchmark("Subdivision (level 1)", sub1);

    GeodesicAlgorithmSubdivision sub3(&mesh, 3);
    benchmark("Subdivision (level 3)", sub3);

    GeodesicAlgorithmSubdivision sub5(&mesh, 5);
    benchmark("Subdivision (level 5)", sub5);

    std::cout << "----------------------------------------------------------" << std::endl;
}

int main(int argc, char** argv) {
    std::string mesh_file = "hedgehog_mesh.txt";
    if (argc > 1) {
        mesh_file = argv[1];
    }
    run_benchmark(mesh_file);
    return 0;
}
