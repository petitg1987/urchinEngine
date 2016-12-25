#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "math/geometry/ShrinkConvexHullTest.h"
#include "AssertHelper.h"

using namespace urchin;

void ShrinkConvexHullTest::shrinkConvexHullBox()
{
	Point3<float> aabbPointsTab[] = {
			Point3<float>(0.0, 1.0, 0.0), Point3<float>(1.0, 1.0, 0.0), Point3<float>(1.0, -1.0, 0.0), Point3<float>(0.0, -1.0, 0.0),
			Point3<float>(0.0, 1.0, -1.0), Point3<float>(1.0, 1.0, -1.0), Point3<float>(1.0, -1.0, -1.0), Point3<float>(0.0, -1.0, -1.0)
	};
	std::vector<Point3<float>> aabbPoints(aabbPointsTab, aabbPointsTab+sizeof(aabbPointsTab)/sizeof(Point3<float>));
	ConvexHull3D<float> convexHull(aabbPoints);

	std::unique_ptr<ConvexHull3D<float>> convexHullResult = convexHull.resize(-0.1f);

	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, 1.0, -1.0)), Point3<float>(0.9, 0.9, -0.9));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, 1.0, 1.0)), Point3<float>(0.9, 0.9, -0.1));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, 1.0, -1.0)), Point3<float>(0.1, 0.9, -0.9));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, 1.0, 1.0)), Point3<float>(0.1, 0.9, -0.1));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, -1.0)), Point3<float>(0.9, -0.9, -0.9));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(1.0, -1.0, 1.0)), Point3<float>(0.9, -0.9, -0.1));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, -1.0)), Point3<float>(0.1, -0.9, -0.9));
	AssertHelper::assertPoint3FloatEquals(convexHullResult->getSupportPoint(Vector3<float>(-1.0, -1.0, 1.0)), Point3<float>(0.1, -0.9, -0.1));
}

CppUnit::Test *ShrinkConvexHullTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("ShrinkConvexHullTest");

	suite->addTest(new CppUnit::TestCaller<ShrinkConvexHullTest>("shrinkConvexHullBox", &ShrinkConvexHullTest::shrinkConvexHullBox));

	return suite;
}
