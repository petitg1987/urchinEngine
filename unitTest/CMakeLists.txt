cmake_minimum_required(VERSION 3.7)
project(unitTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/ai/path/MonotonePolygonTest.cpp
        src/ai/path/MonotonePolygonTest.h
        src/ai/path/PolygonsIntersectionTest.cpp
        src/ai/path/PolygonsIntersectionTest.h
        src/ai/path/PolygonsUnionTest.cpp
        src/ai/path/PolygonsUnionTest.h
        src/ai/path/TriangulationTest.cpp
        src/ai/path/TriangulationTest.h
        src/math/algebra/QuaternionEulerTest.cpp
        src/math/algebra/QuaternionEulerTest.h
        src/math/geometry/AABBoxCollisionTest.cpp
        src/math/geometry/AABBoxCollisionTest.h
        src/math/geometry/ClosestPointTest.cpp
        src/math/geometry/ClosestPointTest.h
        src/math/geometry/ConvexHullShape2DTest.cpp
        src/math/geometry/ConvexHullShape2DTest.h
        src/math/geometry/LineSegment2DCollisionTest.cpp
        src/math/geometry/LineSegment2DCollisionTest.h
        src/math/geometry/OrthogonalProjectionTest.cpp
        src/math/geometry/OrthogonalProjectionTest.h
        src/math/geometry/ResizeConvexHull3DTest.cpp
        src/math/geometry/ResizeConvexHull3DTest.h
        src/math/geometry/SortPointsTest.cpp
        src/math/geometry/SortPointsTest.h
        src/physics/algorithm/broadphase/BodyTestHelper.cpp
        src/physics/algorithm/broadphase/BodyTestHelper.h
        src/physics/algorithm/broadphase/SweepAndPruneTest.cpp
        src/physics/algorithm/broadphase/SweepAndPruneTest.h
        src/physics/algorithm/epa/EPABoxTest.cpp
        src/physics/algorithm/epa/EPABoxTest.h
        src/physics/algorithm/epa/EPAConvexHullTest.cpp
        src/physics/algorithm/epa/EPAConvexHullTest.h
        src/physics/algorithm/epa/EPAConvexObjectTest.cpp
        src/physics/algorithm/epa/EPAConvexObjectTest.h
        src/physics/algorithm/epa/EPASphereTest.cpp
        src/physics/algorithm/epa/EPASphereTest.h
        src/physics/algorithm/epa/EPATestHelper.cpp
        src/physics/algorithm/epa/EPATestHelper.h
        src/physics/algorithm/gjk/GJKBoxTest.cpp
        src/physics/algorithm/gjk/GJKBoxTest.h
        src/physics/algorithm/gjk/GJKConvexHullTest.cpp
        src/physics/algorithm/gjk/GJKConvexHullTest.h
        src/physics/algorithm/gjk/GJKConvexObjectTest.cpp
        src/physics/algorithm/gjk/GJKConvexObjectTest.h
        src/physics/algorithm/gjk/GJKSphereTest.cpp
        src/physics/algorithm/gjk/GJKSphereTest.h
        src/physics/algorithm/gjk/GJKTestHelper.cpp
        src/physics/algorithm/gjk/GJKTestHelper.h
        src/physics/algorithm/inertia/InertiaCalculationTest.cpp
        src/physics/algorithm/inertia/InertiaCalculationTest.h
        src/physics/island/IslandContainerTest.cpp
        src/physics/island/IslandContainerTest.h
        src/physics/object/SupportPointTest.cpp
        src/physics/object/SupportPointTest.h
        src/physics/shape/ShapeToAABBoxTest.cpp
        src/physics/shape/ShapeToAABBoxTest.h
        src/physics/shape/ShapeToConvexObjectTest.cpp
        src/physics/shape/ShapeToConvexObjectTest.h
        src/system/FileHandlerTest.cpp
        src/system/FileHandlerTest.h
        src/AssertHelper.cpp
        src/AssertHelper.h
        src/MainTest.cpp)

include_directories(src ../common/src ../physicsEngine/src ../AIEngine/src)

add_executable(unitTest ${SOURCE_FILES})
target_link_libraries(unitTest pthread cppunit urchinCommon urchinPhysicsEngine urchinAIEngine)