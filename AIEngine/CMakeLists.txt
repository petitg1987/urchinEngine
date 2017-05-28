cmake_minimum_required(VERSION 3.7)
project(urchinAIEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/input/AIObject.cpp
        src/input/AIObject.h
        src/input/AIWorld.cpp
        src/input/AIWorld.h
        src/path/navmesh/csg/CSGPolygon.cpp
        src/path/navmesh/csg/CSGPolygon.h
        src/path/navmesh/csg/PolygonsIntersection.cpp
        src/path/navmesh/csg/PolygonsIntersection.h
        src/path/navmesh/csg/PolygonsUnion.cpp
        src/path/navmesh/csg/PolygonsUnion.h
        src/path/navmesh/model/NavMesh.cpp
        src/path/navmesh/model/NavMesh.h
        src/path/navmesh/model/NavPolygon.cpp
        src/path/navmesh/model/NavPolygon.h
        src/path/navmesh/polyhedron/Polyhedron.cpp
        src/path/navmesh/polyhedron/Polyhedron.h
        src/path/navmesh/polyhedron/PolyhedronFace.cpp
        src/path/navmesh/polyhedron/PolyhedronFace.h
        src/path/navmesh/polyhedron/PolyhedronPoint.cpp
        src/path/navmesh/polyhedron/PolyhedronPoint.h
        src/path/navmesh/triangulation/MonotonePolygon.cpp
        src/path/navmesh/triangulation/MonotonePolygon.h
        src/path/navmesh/triangulation/Triangulation.cpp
        src/path/navmesh/triangulation/Triangulation.h
        src/path/navmesh/NavMeshConfig.cpp
        src/path/navmesh/NavMeshConfig.h
        src/path/navmesh/NavMeshGenerator.cpp
        src/path/navmesh/NavMeshGenerator.h
        src/UrchinAIEngine.h)

include_directories(src ../common/src)

add_library(urchinAIEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinAIEngine urchinCommon -static-libstdc++)