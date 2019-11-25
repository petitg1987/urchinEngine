#include <cppunit/ui/text/TestRunner.h>

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
#include "physics/shape/ShapeToAABBoxTest.h"
#include "physics/shape/ShapeToConvexObjectTest.h"
#include "physics/object/SupportPointTest.h"
#include "physics/algorithm/gjk/GJKBoxTest.h"
#include "physics/algorithm/gjk/GJKConvexHullTest.h"
#include "physics/algorithm/gjk/GJKSphereTest.h"
#include "physics/algorithm/gjk/GJKConvexObjectTest.h"
#include "physics/algorithm/epa/EPABoxTest.h"
#include "physics/algorithm/epa/EPASphereTest.h"
#include "physics/algorithm/epa/EPAConvexHullTest.h"
#include "physics/algorithm/epa/EPAConvexObjectTest.h"
#include "physics/algorithm/inertia/InertiaCalculationTest.h"
#include "physics/island/IslandContainerTest.h"
#include "physics/it/FallingObjectIT.h"
#include "ai/path/navmesh/csg/CSGPolygonTest.h"
#include "ai/path/navmesh/csg/PolygonsUnionTest.h"
#include "ai/path/navmesh/csg/PolygonsSubtractionTest.h"
#include "ai/path/navmesh/triangulation/MonotonePolygonTest.h"
#include "ai/path/navmesh/triangulation/TriangulationTest.h"
#include "ai/path/navmesh/polytope/services/TerrainObstacleServiceTest.h"
#include "ai/path/navmesh/jump/EdgeJumpDetectionTest.h"
#include "ai/path/navmesh/NavMeshGeneratorTest.h"

#include "ai/path/pathfinding/FunnelAlgorithmTest.h"

void commonTests(CppUnit::TextUi::TestRunner &runner)
{
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

void physicsTests(CppUnit::TextUi::TestRunner &runner)
{
    //shape
    runner.addTest(ShapeToAABBoxTest::suite());
    runner.addTest(ShapeToConvexObjectTest::suite());

    //object
    runner.addTest(SupportPointTest::suite());

    //algorithm
    runner.addTest(GJKSphereTest::suite());
    runner.addTest(GJKBoxTest::suite());
    runner.addTest(GJKConvexHullTest::suite());
    runner.addTest(GJKConvexObjectTest::suite());
    runner.addTest(EPABoxTest::suite());
    runner.addTest(EPASphereTest::suite());
    runner.addTest(EPAConvexHullTest::suite());
    runner.addTest(EPAConvexObjectTest::suite());

    //constraint solver
    runner.addTest(InertiaCalculationTest::suite());

    //container
    runner.addTest(IslandContainerTest::suite());

    //integration tests (IT)
    runner.addTest(FallingObjectIT::suite());
}

void aiTests(CppUnit::TextUi::TestRunner &runner)
{
    //navigation mesh
    runner.addTest(CSGPolygonTest::suite());
    runner.addTest(PolygonsUnionTest::suite());
    runner.addTest(PolygonsSubtractionTest::suite());
    runner.addTest(MonotonePolygonTest::suite());
    runner.addTest(TriangulationTest::suite());
    runner.addTest(TerrainObstacleServiceTest::suite());
    runner.addTest(EdgeJumpDetectionTest::suite());
    runner.addTest(NavMeshGeneratorTest::suite());

    //pathfinding
    runner.addTest(FunnelAlgorithmTest::suite());
}

int main()
{
	urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

    CppUnit::TextUi::TestRunner runner;
    commonTests(runner);
    physicsTests(runner);
    aiTests(runner);
	runner.run();

	urchin::SingletonManager::destroyAllSingletons();
	return 0;
}
