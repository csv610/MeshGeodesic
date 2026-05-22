cmake_minimum_required(VERSION 3.10)
project(Geodesic CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
  add_compile_options(/W4)
 endif()

# Source files
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Core library
add_library(geodesic STATIC src/geodesic_algorithm_exact.cpp)
target_include_directories(geodesic PUBLIC ${SOURCE_DIR})

# Executables
add_executable(example0 app/example0.cpp)
target_link_libraries(example0 PRIVATE geodesic)

add_executable(example1 app/example1.cpp)
target_link_libraries(example1 PRIVATE geodesic)

add_executable(farthest_point_sampling app/farthest_point_sampling.cpp)
target_link_libraries(farthest_point_sampling PRIVATE geodesic)

# Unit Tests
enable_testing()
add_executable(test_geodesic tests/test_geodesic.cpp)
target_include_directories(test_geodesic PRIVATE tests)
target_link_libraries(test_geodesic PRIVATE geodesic)
add_test(NAME GeodesicTests COMMAND test_geodesic)

# Benchmarks
add_executable(benchmark_geodesic tests/benchmark_geodesic.cpp)
target_link_libraries(benchmark_geodesic PRIVATE geodesic)

# Optional: MATLAB API
# This requires Boost. If not found, it will be skipped.
find_package(Boost QUIET)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_library(geodesic_matlab_api SHARED src/geodesic_matlab_api.cpp)
    target_link_libraries(geodesic_matlab_api PRIVATE geodesic ${Boost_LIBRARIES})
    
    # On non-Windows platforms, we might need to handle the __declspec(dllexport)
    # which is already handled in geodesic_matlab_api.h by:
    # #ifndef GEODESIC_DLL_IMPORT 
    # #define GEODESIC_DLL_IMPORT
    # #endif
else()
    message(STATUS "Boost not found. Skipping geodesic_matlab_api.")
endif()

# Copy data files to build directory for convenience
file(GLOB DATA_FILES "data/*.txt")
foreach(data_file ${DATA_FILES})
    configure_file(${data_file} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
