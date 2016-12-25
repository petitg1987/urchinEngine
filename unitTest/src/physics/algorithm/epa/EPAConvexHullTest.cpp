#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/algorithm/epa/EPAConvexHullTest.h"
#include "physics/algorithm/epa/EPATestHelper.h"
using namespace urchin;

void EPAConvexHullTest::identicalBox()
{
	Point3<float> obbPointsTab1[] = {
			Point3<float>(-0.3, 1.0, 0.0), Point3<float>(0.2, 0.0, 0.0), Point3<float>(-0.3, -1.0, 0.0), Point3<float>(-0.8, 0.0, 0.0),
			Point3<float>(-0.3, 1.0, -1.0), Point3<float>(0.2, 0.0, -1.0), Point3<float>(-0.3, -1.0, -1.0), Point3<float>(-0.8, 0.0, -1.0)
	};
	std::vector<Point3<float>> obbPoints1(obbPointsTab1, obbPointsTab1+sizeof(obbPointsTab1)/sizeof(Point3<float>));

	Point3<float> obbPointsTab2[] = {
			Point3<float>(-0.3, 1.0, 0.0), Point3<float>(0.2, 0.0, 0.0), Point3<float>(-0.3, -1.0, 0.0), Point3<float>(-0.8, 0.0, 0.0),
			Point3<float>(-0.3, 1.0, -1.0), Point3<float>(0.2, 0.0, -1.0), Point3<float>(-0.3, -1.0, -1.0), Point3<float>(-0.8, 0.0, -1.0)
	};
	std::vector<Point3<float>> obbPoints2(obbPointsTab2, obbPointsTab2+sizeof(obbPointsTab2)/sizeof(Point3<float>));

	CollisionConvexHullObject ch1(0.0, obbPoints1, obbPoints1);
	CollisionConvexHullObject ch2(0.0, obbPoints2, obbPoints2);
	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.894427191);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.894427191);
}

void EPAConvexHullTest::cornerInsideBox()
{
	Point3<float> aabbPointsTab[] = {
			Point3<float>(0.0, 1.0, 0.0), Point3<float>(1.0, 1.0, 0.0), Point3<float>(1.0, -1.0, 0.0), Point3<float>(0.0, -1.0, 0.0),
			Point3<float>(0.0, 1.0, -1.0), Point3<float>(1.0, 1.0, -1.0), Point3<float>(1.0, -1.0, -1.0), Point3<float>(0.0, -1.0, -1.0)
	};
	std::vector<Point3<float>> aabbPoints(aabbPointsTab, aabbPointsTab+sizeof(aabbPointsTab)/sizeof(Point3<float>));

	Point3<float> obbPointsTab[] = {
			Point3<float>(-0.3, 1.0, 0.0), Point3<float>(0.2, 0.0, 0.0), Point3<float>(-0.3, -1.0, 0.0), Point3<float>(-0.8, 0.0, 0.0),
			Point3<float>(-0.3, 1.0, -1.0), Point3<float>(0.2, 0.0, -1.0), Point3<float>(-0.3, -1.0, -1.0), Point3<float>(-0.8, 0.0, -1.0)
	};
	std::vector<Point3<float>> obbPoints(obbPointsTab, obbPointsTab+sizeof(obbPointsTab)/sizeof(Point3<float>));

	CollisionConvexHullObject ch1(0.0, aabbPoints, aabbPoints);
	CollisionConvexHullObject ch2(0.0, obbPoints, obbPoints);
	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.2);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(-1.0, 0.0, 0.0));
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.2);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 0.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.2);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.0);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPAConvexHullTest::faceInsideTrapeze()
{
	Point3<float> trapezePointsTab1[] = {
			Point3<float>(1.0, 0.0, 0.0), Point3<float>(3.0, 0.0, 0.0), Point3<float>(0.0, 3.0, 0.0), Point3<float>(0.0, 1.0, 0.0),
			Point3<float>(1.0, 0.0, -1.0), Point3<float>(3.0, 0.0, -1.0), Point3<float>(0.0, 3.0, -1.0), Point3<float>(0.0, 1.0, -1.0),
	};
	std::vector<Point3<float>> trapezePoints1(trapezePointsTab1, trapezePointsTab1+sizeof(trapezePointsTab1)/sizeof(Point3<float>));

	Point3<float> trapezePointsTab2[] = {
			Point3<float>(0.0, -1.0, 0.0), Point3<float>(1.0, 0.8, 0.0), Point3<float>(0.8, 1.0, 0.0), Point3<float>(-1.0, 0.0, 0.0),
			Point3<float>(0.0, -1.0, -1.0), Point3<float>(1.0, 0.8, -1.0), Point3<float>(0.8, 1.0, -1.0), Point3<float>(-1.0, 0.0, -1.0),
	};
	std::vector<Point3<float>> trapezePoints2(trapezePointsTab2, trapezePointsTab2+sizeof(trapezePointsTab2)/sizeof(Point3<float>));

	CollisionConvexHullObject ch1(0.0, trapezePoints1, trapezePoints1);
	CollisionConvexHullObject ch2(0.0, trapezePoints2, trapezePoints2);
	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.56568542494);
	AssertHelper::assertVector3FloatEquals(resultEpa->getNormal(), Vector3<float>(-0.70710678118, -0.70710678118, 0.0));
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.56568542494);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().X, 0.5);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Y, 0.5);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().X, 0.9);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointB().Y, 0.9);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

void EPAConvexHullTest::cornerInsideHexagon()
{
	Point3<float> hexagonPointsTab1[] = {
			Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(1.5, 1.0, 0.0), Point3<float>(1.0, 2.0, 0.0), Point3<float>(0.0, 2.0, 0.0), Point3<float>(-0.5, 1.0, 0.0),
			Point3<float>(0.0, 0.0, -1.0), Point3<float>(1.0, 0.0, -1.0), Point3<float>(1.5, 1.0, -1.0), Point3<float>(1.0, 2.0, -1.0), Point3<float>(0.0, 2.0, -1.0), Point3<float>(-0.5, 1.0, -1.0),
	};
	std::vector<Point3<float>> hexagonPoints1(hexagonPointsTab1, hexagonPointsTab1+sizeof(hexagonPointsTab1)/sizeof(Point3<float>));

	Point3<float> hexagonPointsTab2[] = {
			Point3<float>(-1.5, 0.0, 0.0), Point3<float>(-0.5, 0.0, 0.0), Point3<float>(0.0, 1.0, 0.0), Point3<float>(-0.5, 2.0, 0.0), Point3<float>(-1.5, 2.0, 0.0), Point3<float>(-2.0, 1.0, 0.0),
			Point3<float>(-1.5, 0.0, -1.0), Point3<float>(-0.5, 0.0, -1.0), Point3<float>(0.0, 1.0, -1.0), Point3<float>(-0.5, 2.0, -1.0), Point3<float>(-1.5, 2.0, -1.0), Point3<float>(-2.0, 1.0, -1.0),
	};
	std::vector<Point3<float>> hexagonPoints2(hexagonPointsTab2, hexagonPointsTab2+sizeof(hexagonPointsTab2)/sizeof(Point3<float>));

	CollisionConvexHullObject ch1(0.0, hexagonPoints1, hexagonPoints1);
	CollisionConvexHullObject ch2(0.0, hexagonPoints2, hexagonPoints2);
	std::shared_ptr<EPAResult<float>> resultEpa = EPATestHelper::executeEPA(ch1, ch2);

	AssertHelper::assertTrue(resultEpa->isCollide());
	AssertHelper::assertFloatEquals(resultEpa->getPenetrationDepth(), 0.44721359);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().vector(resultEpa->getContactPointB()).length(), 0.44721359);
	AssertHelper::assertFloatEquals(resultEpa->getContactPointA().Z, resultEpa->getContactPointB().Z);
}

CppUnit::Test *EPAConvexHullTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("EPAConvexHullTest");

	suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("identicalBox", &EPAConvexHullTest::identicalBox));
	suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("cornerInsideBox", &EPAConvexHullTest::cornerInsideBox));

	suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("faceInsideTrapeze", &EPAConvexHullTest::faceInsideTrapeze));

	suite->addTest(new CppUnit::TestCaller<EPAConvexHullTest>("cornerInsideHexagon", &EPAConvexHullTest::cornerInsideHexagon));

	return suite;
}
