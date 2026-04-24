#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <iomanip>
#include <cmath>

#include "geodesic_algorithm_exact.h"
#include "geodesic_algorithm_dijkstra.h"

void hsv_to_rgb(double h, double s, double v, unsigned& r, unsigned& g, unsigned& b) {
    if (s < 1e-10) {
        r = g = b = static_cast<unsigned>(v * 255.0);
        return;
    }
    double hh = h * 6.0;
    int ii = static_cast<int>(hh);
    double ff = hh - ii;
    double p = v * (1.0 - s);
    double q = v * (1.0 - s * ff);
    double t = v * (1.0 - s * (1.0 - ff));
    switch (ii % 6) {
        case 0: r = static_cast<unsigned>(v * 255.0); g = static_cast<unsigned>(t * 255.0); b = static_cast<unsigned>(p * 255.0); break;
        case 1: r = static_cast<unsigned>(q * 255.0); g = static_cast<unsigned>(v * 255.0); b = static_cast<unsigned>(p * 255.0); break;
        case 2: r = static_cast<unsigned>(p * 255.0); g = static_cast<unsigned>(v * 255.0); b = static_cast<unsigned>(t * 255.0); break;
        case 3: r = static_cast<unsigned>(p * 255.0); g = static_cast<unsigned>(q * 255.0); b = static_cast<unsigned>(v * 255.0); break;
        case 4: r = static_cast<unsigned>(t * 255.0); g = static_cast<unsigned>(p * 255.0); b = static_cast<unsigned>(v * 255.0); break;
        default: r = static_cast<unsigned>(v * 255.0); g = static_cast<unsigned>(p * 255.0); b = static_cast<unsigned>(q * 255.0); break;
    }
}

void print_mesh_info(geodesic::Mesh& mesh) {
    std::cout << "mesh has " << mesh.vertices().size() << " vertices, "
              << mesh.faces().size() << " faces, "
              << mesh.edges().size() << " edges" << std::endl;
}

void print_ply_header(std::ofstream& out, std::size_t num_vertices, std::size_t num_faces) {
    out << "ply" << std::endl;
    out << "format binary_little_endian 1.0" << std::endl;
    out << "element vertex " << num_vertices << std::endl;
    out << "property float x" << std::endl;
    out << "property float y" << std::endl;
    out << "property float z" << std::endl;
    out << "property uchar red" << std::endl;
    out << "property uchar green" << std::endl;
    out << "property uchar blue" << std::endl;
    out << "element face " << num_faces << std::endl;
    out << "property list uchar int vertex_indices" << std::endl;
    out << "end_header" << std::endl;
}

std::tuple<double, double, double> compute_min_max_distance(geodesic::Mesh& mesh,
                                               geodesic::GeodesicAlgorithmBase* algorithm) {
    double min_dist = geodesic::GEODESIC_INF;
    double max_dist = 0.0;

    for (std::size_t i = 0; i < mesh.vertices().size(); ++i) {
        geodesic::SurfacePoint point(&mesh.vertices()[i]);
        double dist;
        algorithm->best_source(point, dist);

        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    return {min_dist, max_dist, 0.0};
}

std::tuple<unsigned, double> find_farthest_vertex(geodesic::Mesh& mesh,
                                          geodesic::GeodesicAlgorithmBase* algorithm,
                                          std::vector<geodesic::SurfacePoint>& sources) {
    algorithm->propagate(sources);

    unsigned farthest_idx = 0;
    double farthest_dist = 0.0;

    for (std::size_t i = 0; i < mesh.vertices().size(); ++i) {
        geodesic::SurfacePoint point(&mesh.vertices()[i]);
        double dist;
        algorithm->best_source(point, dist);

        if (dist > farthest_dist) {
            farthest_dist = dist;
            farthest_idx = static_cast<unsigned>(i);
        }
    }

    return {farthest_idx, farthest_dist};
}

std::vector<unsigned> farthest_point_sampling(geodesic::Mesh& mesh,
                                         geodesic::GeodesicAlgorithmBase* algorithm,
                                         std::size_t num_samples,
                                         unsigned random_seed = 42) {
    std::vector<unsigned> sampled_vertices;

    std::mt19937 rng(random_seed);
    std::uniform_int_distribution<std::size_t> dist(0, mesh.vertices().size() - 1);

    std::vector<geodesic::SurfacePoint> sources;

    std::size_t initial_idx = dist(rng);
    geodesic::SurfacePoint initial_source(&mesh.vertices()[initial_idx]);
    sources.push_back(initial_source);
    sampled_vertices.push_back(static_cast<unsigned>(initial_idx));

    std::cout << "Initial source: vertex " << initial_idx << std::endl;

    while (sampled_vertices.size() < num_samples) {
        auto [farthest_idx, farthest_dist] = find_farthest_vertex(mesh, algorithm, sources);

        std::cout << "Sampled vertex " << farthest_idx
                 << " (distance: " << farthest_dist << ")" << std::endl;

        sampled_vertices.push_back(farthest_idx);
        sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[farthest_idx]));
    }

    return sampled_vertices;
}

void write_colored_ply(const std::string& filename,
                    geodesic::Mesh& mesh,
                    geodesic::GeodesicAlgorithmBase* algorithm,
                    std::vector<geodesic::SurfacePoint>& sources) {
    algorithm->propagate(sources);

    auto [min_dist, max_dist, _] = compute_min_max_distance(mesh, algorithm);
    double range = max_dist - min_dist;
    if (range < 1e-10) range = 1.0;

    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    print_ply_header(out, mesh.vertices().size(), mesh.faces().size());

    auto& vertices = mesh.vertices();
    auto& faces = mesh.faces();

    for (std::size_t i = 0; i < vertices.size(); ++i) {
        auto& v = vertices[i];
        float x = static_cast<float>(v.x());
        float y = static_cast<float>(v.y());
        float z = static_cast<float>(v.z());

        geodesic::SurfacePoint point(&vertices[i]);
        double dist;
        algorithm->best_source(point, dist);

        double t = (dist - min_dist) / range;
        t = std::max(0.0, std::min(1.0, t));

        unsigned r, g, b;
        hsv_to_rgb(0.66 - t * 0.66, 1.0, 1.0, r, g, b);

        out.write(reinterpret_cast<const char*>(&x), sizeof(float));
        out.write(reinterpret_cast<const char*>(&y), sizeof(float));
        out.write(reinterpret_cast<const char*>(&z), sizeof(float));
        out.write(reinterpret_cast<const char*>(&r), sizeof(unsigned char));
        out.write(reinterpret_cast<const char*>(&g), sizeof(unsigned char));
        out.write(reinterpret_cast<const char*>(&b), sizeof(unsigned char));
    }

    for (std::size_t i = 0; i < faces.size(); ++i) {
        auto& f = faces[i];
        unsigned char n = 3;
        unsigned i0 = f.adjacent_vertices()[0]->id();
        unsigned i1 = f.adjacent_vertices()[1]->id();
        unsigned i2 = f.adjacent_vertices()[2]->id();

        out.write(reinterpret_cast<const char*>(&n), sizeof(unsigned char));
        out.write(reinterpret_cast<const char*>(&i0), sizeof(unsigned));
        out.write(reinterpret_cast<const char*>(&i1), sizeof(unsigned));
        out.write(reinterpret_cast<const char*>(&i2), sizeof(unsigned));
    }

    out.close();
    std::cout << "Colored PLY saved to: " << filename << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "usage: " << argv[0]
                  << " <mesh_file> <num_samples> [output_ply]" << std::endl;
        std::cout << "  mesh_file:    Input mesh file (.txt)" << std::endl;
        std::cout << "  num_samples: Number of samples to collect" << std::endl;
        std::cout << "  output_ply: Output PLY file (optional)" << std::endl;
        return 0;
    }

    std::string mesh_file = argv[1];
    std::size_t num_samples = std::stoul(argv[2]);
    std::string output_ply = (argc > 3) ? argv[3] : "output.ply";

    std::vector<double> points;
    std::vector<unsigned> faces;

    if (!geodesic::read_mesh_from_file(mesh_file.c_str(), points, faces)) {
        std::cerr << "Failed to read mesh: " << mesh_file << std::endl;
        return 1;
    }

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces);

    print_mesh_info(mesh);

    if (num_samples > mesh.vertices().size()) {
        std::cerr << "Error: num_samples (" << num_samples << ") > num_vertices ("
                  << mesh.vertices().size() << ")" << std::endl;
        return 1;
    }

    geodesic::GeodesicAlgorithmDijkstra algorithm(&mesh);

    std::cout << "\n=== Farthest Point Sampling ===" << std::endl;
    auto sampled = farthest_point_sampling(mesh, &algorithm, num_samples);

    std::cout << "\nSampled vertices: ";
    for (std::size_t i = 0; i < sampled.size(); ++i) {
        std::cout << sampled[i];
        if (i < sampled.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    std::cout << "\n=== Writing colored PLY ===" << std::endl;

    std::vector<geodesic::SurfacePoint> sources;
    for (unsigned idx : sampled) {
        sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[idx]));
    }

    write_colored_ply(output_ply, mesh, &algorithm, sources);

    std::cout << "\nDone!" << std::endl;

    return 0;
}
