#include <cppunit/ui/text/TestRunner.h>
#include "UrchinCommon.h"

#include "common/io/StringUtilTest.h"
#include "common/io/MapUtilTest.h"
#include "common/system/FileHandlerTest.h"
#include "common/math/algebra/QuaternionTest.h"
#include "common/math/geometry/OrthogonalProjectionTest.h"
#include "common/math/geometry/ClosestPointTest.h"
#include "common/math/geometry/AABBoxCollisionTest.h"
#include "common/math/geometry/LineSegment2DCollisionTest.h"
#include "common/math/geometry/ResizeConvexHull3DTest.h"
#include "common/math/geometry/ResizePolygon2DServiceTest.h"
#include "common/math/geometry/ConvexHullShape2DTest.h"
#include "common/math/geometry/SortPointsTest.h"
#include "3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h"
#include "physics/shape/ShapeToAABBoxTest.h"
#include "physics/shape/ShapeToConvexObjectTest.h"
#include "physics/object/SupportPointTest.h"
#include "physics/body/InertiaCalculationTest.h"
#include "physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKBoxTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKConvexHullTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKSphereTest.h"
#include "physics/collision/narrowphase/algorithm/gjk/GJKConvexObjectTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPABoxTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPASphereTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPAConvexHullTest.h"
#include "physics/collision/narrowphase/algorithm/epa/EPAConvexObjectTest.h"
#include "physics/collision/island/IslandContainerTest.h"
#include "physics/collision/FallingObjectIT.h"
#include "ai/path/navmesh/csg/CSGPolygonTest.h"
#include "ai/path/navmesh/csg/PolygonsUnionTest.h"
#include "ai/path/navmesh/csg/PolygonsSubtractionTest.h"
#include "ai/path/navmesh/triangulation/MonotonePolygonTest.h"
#include "ai/path/navmesh/triangulation/TriangulationTest.h"
#include "ai/path/navmesh/polytope/services/TerrainObstacleServiceTest.h"
#include "ai/path/navmesh/jump/EdgeLinkDetectionTest.h"
#include "ai/path/navmesh/NavMeshGeneratorTest.h"
#include "ai/path/pathfinding/FunnelAlgorithmTest.h"
#include "ai/path/pathfinding/PathfindingAStarTest.h"

void commonTests(CppUnit::TextUi::TestRunner& runner) {
    //io
    runner.addTest(StringUtilTest::suite());
    runner.addTest(MapUtilTest::suite());

    //system - file
    runner.addTest(FileHandlerTest::suite());

    //math - algebra
    runner.addTest(QuaternionTest::suite());

    //math - geometry
    runner.addTest(OrthogonalProjectionTest::suite());
    runner.addTest(ClosestPointTest::suite());
    runner.addTest(AABBoxCollisionTest::suite());
    runner.addTest(LineSegment2DCollisionTest::suite());
    runner.addTest(ResizeConvexHull3DTest::suite());
    runner.addTest(ResizePolygon2DServiceTest::suite());
    runner.addTest(ConvexHullShape2DTest::suite());
    runner.addTest(SortPointsTest::suite());
}

void engine3dTests(CppUnit::TextUi::TestRunner& runner) {
    //shadow
    runner.addTest(LightSplitShadowMapTest::suite());
}

void physicsTests(CppUnit::TextUi::TestRunner& runner) {
    //shape
    runner.addTest(ShapeToAABBoxTest::suite());
    runner.addTest(ShapeToConvexObjectTest::suite());

    //object
    runner.addTest(SupportPointTest::suite());

    //body
    runner.addTest(InertiaCalculationTest::suite());

    //broad phase
    runner.addTest(BodyAABBTreeTest::suite());

    //narrow phase
    runner.addTest(GJKSphereTest::suite());
    runner.addTest(GJKBoxTest::suite());
    runner.addTest(GJKConvexHullTest::suite());
    runner.addTest(GJKConvexObjectTest::suite());
    runner.addTest(EPABoxTest::suite());
    runner.addTest(EPASphereTest::suite());
    runner.addTest(EPAConvexHullTest::suite());
    runner.addTest(EPAConvexObjectTest::suite());

    //island
    runner.addTest(IslandContainerTest::suite());

    //integration tests (IT)
    runner.addTest(FallingObjectIT::suite());
}

void aiTests(CppUnit::TextUi::TestRunner& runner) {
    //navigation mesh
    runner.addTest(CSGPolygonTest::suite());
    runner.addTest(PolygonsUnionTest::suite());
    runner.addTest(PolygonsSubtractionTest::suite());
    runner.addTest(MonotonePolygonTest::suite());
    runner.addTest(TriangulationTest::suite());
    runner.addTest(TerrainObstacleServiceTest::suite());
    runner.addTest(EdgeLinkDetectionTest::suite());
    runner.addTest(NavMeshGeneratorTest::suite());

    //pathfinding
    runner.addTest(FunnelAlgorithmTest::suite());
    runner.addTest(PathfindingAStarTest::suite());
}

int main() {
    urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

    CppUnit::TextUi::TestRunner runner;
    commonTests(runner);
    engine3dTests(runner);
    physicsTests(runner);
    aiTests(runner);
    runner.run();

    urchin::SingletonManager::destroyAllSingletons();
    return 0;
}
