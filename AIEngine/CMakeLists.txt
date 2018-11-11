cmake_minimum_required(VERSION 3.7)
project(urchinAIEngine)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_STANDARD 14)

add_definitions(-ffast-math)

set(SOURCE_FILES
        src/input/AIObject.cpp
        src/input/AIObject.h
        src/input/AIWorld.cpp
        src/input/AIWorld.h
        src/path/navmesh/csg/CSGPolygon.cpp
        src/path/navmesh/csg/CSGPolygon.h
        src/path/navmesh/csg/PolygonsUnion.cpp
        src/path/navmesh/csg/PolygonsUnion.h
        src/path/navmesh/model/NavMeshConfig.cpp
        src/path/navmesh/model/NavMeshConfig.h
        src/path/navmesh/model/NavMesh.cpp
        src/path/navmesh/model/NavMesh.h
        src/path/navmesh/model/NavPolygon.cpp
        src/path/navmesh/model/NavPolygon.h
        src/path/navmesh/polytope/Polytope.cpp
        src/path/navmesh/polytope/Polytope.h
        src/path/navmesh/polytope/PolytopePlaneSurface.cpp
        src/path/navmesh/polytope/PolytopePlaneSurface.h
        src/path/navmesh/triangulation/MonotonePolygonAlgorithm.cpp
        src/path/navmesh/triangulation/MonotonePolygonAlgorithm.h
        src/path/navmesh/triangulation/TriangulationAlgorithm.cpp
        src/path/navmesh/triangulation/TriangulationAlgorithm.h
        src/path/navmesh/NavMeshGenerator.cpp
        src/path/navmesh/NavMeshGenerator.h
        src/UrchinAIEngine.h src/path/navmesh/model/NavMeshAgent.cpp
        src/path/navmesh/model/NavMeshAgent.h
        src/path/navmesh/csg/PolygonsSubtraction.cpp
        src/path/navmesh/csg/PolygonsSubtraction.h
        src/AIManager.cpp
        src/AIManager.h
        src/path/navmesh/triangulation/MonotonePolygon.cpp
        src/path/navmesh/triangulation/MonotonePolygon.h
        src/path/navmesh/model/NavTriangle.cpp
        src/path/navmesh/model/NavTriangle.h
        src/path/pathfinding/PathfindingAStar.cpp
        src/path/pathfinding/PathfindingAStar.h
        src/path/pathfinding/PathNode.cpp
        src/path/pathfinding/PathNode.h
        src/path/pathfinding/FunnelAlgorithm.cpp
        src/path/pathfinding/FunnelAlgorithm.h
        src/path/PathRequest.cpp
        src/path/PathRequest.h
        src/input/AIShape.cpp
        src/input/AIShape.h
        src/path/navmesh/polytope/PolytopeBuilder.cpp
        src/path/navmesh/polytope/PolytopeBuilder.h
        src/input/AITerrain.cpp
        src/input/AITerrain.h
        src/input/AIEntity.cpp
        src/input/AIEntity.h
        src/path/navmesh/polytope/PolytopeSurface.cpp
        src/path/navmesh/polytope/PolytopeSurface.h
        src/path/navmesh/polytope/PolytopeTerrainSurface.cpp
        src/path/navmesh/polytope/PolytopeTerrainSurface.h src/path/navmesh/polytope/services/TerrainObstacleService.cpp src/path/navmesh/polytope/services/TerrainObstacleService.h src/path/pathfinding/PathPortal.cpp src/path/pathfinding/PathPortal.h src/path/navmesh/model/topography/NavTopography.cpp src/path/navmesh/model/topography/NavTopography.h src/path/navmesh/model/topography/NavTerrainTopography.cpp src/path/navmesh/model/topography/NavTerrainTopography.h src/path/navmesh/model/topography/NavFlatTopography.cpp src/path/navmesh/model/topography/NavFlatTopography.h src/path/PathPoint.cpp src/path/PathPoint.h src/path/navmesh/csg/CSGPolygonPath.cpp src/path/navmesh/csg/CSGPolygonPath.h)

include_directories(src ../common/src)

add_library(urchinAIEngine SHARED ${SOURCE_FILES})
target_link_libraries(urchinAIEngine urchinCommon -static-libstdc++)