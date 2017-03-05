#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "math/geometry/ResizeConvexHullTest.h"
#include "AssertHelper.h"

using namespace urchin;

void ResizeConvexHullTest::reduceConvexHullBox()
{
	std::vector<Point3<float>> aabbPoints = {
			Point3<float>(0.0, 1.0, 0.0), Point3<float>(1.0, 1.0, 0.0), Point3<float>(1.0, -1.0, 0.0), Point3<float>(0.0, -1.0, 0.0),
			Point3<float>(0.0, 1.0, -1.0), Point3<float>(1.0, 1.0, -1.0), Point3<float>(1.0, -1.0, -1.0), Point3<float>(0.0, -1.0, -1.0)};
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

CppUnit::Test *ResizeConvexHullTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("ResizeConvexHullTest");

	suite->addTest(new CppUnit::TestCaller<ResizeConvexHullTest>("reduceConvexHullBox", &ResizeConvexHullTest::reduceConvexHullBox));

	return suite;
}
