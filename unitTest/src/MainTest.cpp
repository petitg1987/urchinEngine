#include <iostream>
#include <cppunit/ui/text/TestRunner.h>

#include "system/FileHandlerTest.h"
#include "math/algebra/QuaternionTest.h"
#include "math/geometry/OrthogonalProjectionTest.h"
#include "math/geometry/ClosestPointTest.h"
#include "math/geometry/AABBoxCollisionTest.h"
#include "math/geometry/LineSegment2DCollisionTest.h"
#include "math/geometry/ResizeConvexHull3DTest.h"
#include "math/geometry/ResizePolygon2DServiceTest.h"
#include "math/geometry/ConvexHullShape2DTest.h"
#include "math/geometry/SortPointsTest.h"
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
#include "ai/path/navmesh/MonotonePolygonTest.h"
#include "ai/path/navmesh/TriangulationTest.h"
#include "ai/path/navmesh/PolygonsUnionTest.h"
#include "ai/path/navmesh/PolygonsIntersectionTest.h"
#include "ai/path/navmesh/PolygonsSubtractionTest.h"
#include "ai/path/navmesh/NavMeshGeneratorTest.h"
#include "ai/path/navmesh/TerrainObstacleServiceTest.h"
#include "ai/path/pathfinding/FunnelAlgorithmTest.h"

int main()
{
	CppUnit::TextUi::TestRunner runner;

	//engine configuration
	urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

	//system - file
	runner.addTest(FileHandlerTest::suite());

	//math - algebra
	runner.addTest(QuaternionTest::suite());

	//math - geometry
	runner.addTest(OrthogonalProjectionTest::suite());
	runner.addTest(ClosestPointTest::suite());
	runner.addTest(AABBoxCollissionTest::suite());
	runner.addTest(LineSegment2DCollisionTest::suite());
	runner.addTest(ResizeConvexHull3DTest::suite());
	runner.addTest(ResizePolygon2DServiceTest::suite());
	runner.addTest(ConvexHullShape2DTest::suite());
	runner.addTest(SortPointsTest::suite());

	//physics - shape
	runner.addTest(ShapeToAABBoxTest::suite());
	runner.addTest(ShapeToConvexObjectTest::suite());

	//physics - object
	runner.addTest(SupportPointTest::suite());

	//physics - algorithm
	runner.addTest(GJKSphereTest::suite());
	runner.addTest(GJKBoxTest::suite());
	runner.addTest(GJKConvexHullTest::suite());
	runner.addTest(GJKConvexObjectTest::suite());

	runner.addTest(EPABoxTest::suite());
	runner.addTest(EPASphereTest::suite());
	runner.addTest(EPAConvexHullTest::suite());
	runner.addTest(EPAConvexObjectTest::suite());

	//physics - constraint solver
	runner.addTest(InertiaCalculationTest::suite());

	//physics - container
	runner.addTest(IslandContainerTest::suite());

	//ai - navigation mesh
	runner.addTest(MonotonePolygonTest::suite());
	runner.addTest(TriangulationTest::suite());
	runner.addTest(PolygonsUnionTest::suite());
	runner.addTest(PolygonsIntersectionTest::suite());
	runner.addTest(PolygonsSubtractionTest::suite());
	runner.addTest(NavMeshGeneratorTest::suite());
	runner.addTest(TerrainObstacleServiceTest::suite());

	//ai - pathfinding
	runner.addTest(FunnelAlgorithmTest::suite());

	runner.run();

	urchin::SingletonManager::destroyAllSingletons();
	return 0;
}
