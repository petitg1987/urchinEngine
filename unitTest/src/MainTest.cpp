#include <iostream>
#include <cppunit/ui/text/TestRunner.h>

#include "system/FileHandlerTest.h"
#include "math/algebra/QuaternionEulerTest.h"
#include "math/geometry/OrthogonalProjectionTest.h"
#include "math/geometry/ClosestPointTest.h"
#include "math/geometry/AABBoxCollisionTest.h"
#include "math/geometry/ResizeConvexHullTest.h"
#include "math/geometry/SortPointsTest.h"
#include "physics/shape/ShapeToAABBoxTest.h"
#include "physics/shape/ShapeToConvexObjectTest.h"
#include "physics/object/SupportPointTest.h"
#include "physics/algorithm/broadphase/SweepAndPruneTest.h"
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
#include "ai/path/MonotonePolygonTest.h"
#include "ai/path/TriangulationTest.h"

int main()
{
	CppUnit::TextUi::TestRunner runner;

	//engine configuration
	urchin::ConfigService::instance()->loadProperties("resources/engine.properties");

	//system - file
	runner.addTest(FileHandlerTest::suite());

	//math - algebra
	runner.addTest(QuaternionEulerTest::suite());

	//math - geometry
	runner.addTest(OrthogonalProjectionTest::suite());
	runner.addTest(ClosestPointTest::suite());
	runner.addTest(AABBoxCollissionTest::suite());
	runner.addTest(ResizeConvexHullTest::suite());
	runner.addTest(SortPointsTest::suite());

	//physics - shape
	runner.addTest(ShapeToAABBoxTest::suite());
	runner.addTest(ShapeToConvexObjectTest::suite());

	//physics - object
	runner.addTest(SupportPointTest::suite());

	//physics - algorithm
	runner.addTest(SweepAndPruneTest::suite());

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

	//ai - path
	runner.addTest(MonotonePolygonTest::suite());
	runner.addTest(TriangulationTest::suite());

	runner.run();

	urchin::SingletonManager::destroyAllSingletons();
	return 0;
}
