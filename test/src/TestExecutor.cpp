#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <UrchinCommon.h>

#include <common/io/map/MapSerializerTest.h>
#include <common/system/SystemInfoTest.h>
#include <common/util/StringUtilTest.h>
#include <common/util/FileUtilTest.h>
#include <common/math/algebra/QuaternionTest.h>
#include <common/math/algebra/point/PointTest.h>
#include <common/math/algebra/vector/VectorTest.h>
#include <common/math/geometry/OrthogonalProjectionTest.h>
#include <common/math/geometry/ClosestPointTest.h>
#include <common/math/geometry/AABBoxCollisionTest.h>
#include <common/math/geometry/LineSegment2DCollisionTest.h>
#include <common/math/geometry/ResizeConvexHull3DTest.h>
#include <common/math/geometry/ResizePolygon2DServiceTest.h>
#include <common/math/geometry/ConvexHullShape2DTest.h>
#include <common/math/geometry/SortPointsTest.h>
#include <3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h>
#include <physics/shape/ShapeToAABBoxTest.h>
#include <physics/shape/ShapeToConvexObjectTest.h>
#include <physics/object/SupportPointTest.h>
#include <physics/body/BodyManagerTest.h>
#include <physics/body/InertiaCalculationTest.h>
#include <physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKBoxTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKConvexHullTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKSphereTest.h>
#include <physics/collision/narrowphase/algorithm/gjk/GJKConvexObjectTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPABoxTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPASphereTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAConvexHullTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAConvexObjectTest.h>
#include <physics/collision/island/IslandContainerTest.h>
#include <physics/collision/CollisionWorldIT.h>
#include <physics/character/CharacterControllerIT.h>
#include <physics/character/CharacterControllerMT.h>
#include <ai/path/navmesh/csg/CSGPolygonTest.h>
#include <ai/path/navmesh/csg/PolygonsUnionTest.h>
#include <ai/path/navmesh/csg/PolygonsSubtractionTest.h>
#include <ai/path/navmesh/triangulation/TriangulationTest.h>
#include <ai/path/navmesh/polytope/services/TerrainObstacleServiceTest.h>
#include <ai/path/navmesh/jump/EdgeLinkDetectionTest.h>
#include <ai/path/navmesh/NavMeshGeneratorTest.h>
#include <ai/path/pathfinding/FunnelAlgorithmTest.h>
#include <ai/path/pathfinding/PathfindingAStarTest.h>

void addCommonUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //io
    runner.addTest(MapSerializerTest::suite());

    //system
    runner.addTest(SystemInfoTest::suite());

    //util
    runner.addTest(FileUtilTest::suite());
    runner.addTest(StringUtilTest::suite());

    //math - algebra
    runner.addTest(QuaternionTest::suite());
    runner.addTest(PointTest::suite());
    runner.addTest(VectorTest::suite());

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

void add3dUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //shadow
    runner.addTest(LightSplitShadowMapTest::suite());
}

void addPhysicsUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //shape
    runner.addTest(ShapeToAABBoxTest::suite());
    runner.addTest(ShapeToConvexObjectTest::suite());

    //object
    runner.addTest(SupportPointTest::suite());

    //body
    runner.addTest(BodyManagerTest::suite());
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
}

void addPhysicsIntegrationTests(CppUnit::TextUi::TestRunner& runner) {
    //collision world
    runner.addTest(CollisionWorldIT::suite());

    //character
    runner.addTest(CharacterControllerIT::suite());
}

void addPhysicsMonkeyTests(CppUnit::TextUi::TestRunner& runner) {
    //character
    runner.addTest(CharacterControllerMT::suite());
}

void addAiUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //navigation mesh
    runner.addTest(CSGPolygonTest::suite());
    runner.addTest(PolygonsUnionTest::suite());
    runner.addTest(PolygonsSubtractionTest::suite());
    runner.addTest(TriangulationTest::suite());
    runner.addTest(TerrainObstacleServiceTest::suite());
    runner.addTest(EdgeLinkDetectionTest::suite());
    runner.addTest(NavMeshGeneratorTest::suite());

    //pathfinding
    runner.addTest(FunnelAlgorithmTest::suite());
    runner.addTest(PathfindingAStarTest::suite());
}

void addAllUnitTests(CppUnit::TextUi::TestRunner& runner) {
    addCommonUnitTests(runner);
    add3dUnitTests(runner);
    addPhysicsUnitTests(runner);
    addAiUnitTests(runner);
}

void addAllIntegrationTests(CppUnit::TextUi::TestRunner& runner) {
    addPhysicsIntegrationTests(runner);
}

void addAllMonkeyTests(CppUnit::TextUi::TestRunner& runner) {
    addPhysicsMonkeyTests(runner);
}

int main(int argc, char *argv[]) {
    urchin::ConfigService::instance()->loadProperties("resources/engine.properties");
    CppUnit::TextUi::TestRunner runner;

    std::string hasUnitTests = "no";
    std::string hasIntegrationTests = "no";
    std::string hasMonkeyTests = "no";

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "unit") == 0) {
            addAllUnitTests(runner);
            hasUnitTests = "yes";
        } else if (std::strcmp(argv[i], "integration") == 0) {
            addAllIntegrationTests(runner);
            hasIntegrationTests = "yes";
        } else if (std::strcmp(argv[i], "monkey") == 0) {
            addAllMonkeyTests(runner);
            hasMonkeyTests = "yes";
        } else {
            throw std::invalid_argument("Unknown program argument: " + std::string(argv[i]));
        }
    }

    std::cout << "Start running tests (unit: "<< hasUnitTests << ", integration: " << hasIntegrationTests << ", monkey: " << hasMonkeyTests << ")" << std::endl;
    runner.run();

    urchin::Logger::instance()->purge();
    urchin::SingletonManager::destroyAllSingletons();
    return 0;
}
