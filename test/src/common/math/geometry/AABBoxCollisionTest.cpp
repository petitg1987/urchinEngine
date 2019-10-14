#include <cppunit/extensions/HelperMacros.h>

#include "common/math/geometry/AABBoxCollisionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void AABBoxCollissionTest::rayRightToBox()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(2.0, 0.5, 2.0), Vector3<float>(1.0, 0.0, 0.0), 10.0);

	AssertHelper::assertTrue(!box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayRightTopToBox()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(1.5, 2.0, 0.5), Vector3<float>(1.0, 1.0, 0.0), 10.0);

	AssertHelper::assertTrue(!box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayInsideBox()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(0.1, 0.1, 0.1), Vector3<float>(1.0, 1.0, 1.0), 1.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayThroughXPlanes()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(-1.0, 0.5, 0.5), Vector3<float>(1.0, 0.0, 0.0), 10.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayThroughYPlanes()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(0.5, 2.0, 0.5), Vector3<float>(0.0, -1.0, 0.0), 2.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayThroughZPlanes()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(0.5, 0.5, 2.0), Vector3<float>(0.0, 0.0, -1.0), 10.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayThroughXYPlanes()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(1.5, 2.0, 0.5), Vector3<float>(-1.0, -1.0, 0.0), 4.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxCollissionTest::rayInsideToXPlane()
{
	AABBox<float> box(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0));
	Ray<float> ray(Point3<float>(0.5, 0.5, 0.5), Vector3<float>(1.0, 0.0, 0.0), 5.0);

	AssertHelper::assertTrue(box.collideWithRay(ray));
}

CppUnit::Test *AABBoxCollissionTest::suite()
{
    auto *suite = new CppUnit::TestSuite("AABBoxCollissionTest");

	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayRightToBox", &AABBoxCollissionTest::rayRightToBox));
	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayRightTopToBox", &AABBoxCollissionTest::rayRightTopToBox));

	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayInsideBox", &AABBoxCollissionTest::rayInsideBox));

	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayThroughXPlanes", &AABBoxCollissionTest::rayThroughXPlanes));
	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayThroughYPlanes", &AABBoxCollissionTest::rayThroughYPlanes));
	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayThroughZPlanes", &AABBoxCollissionTest::rayThroughZPlanes));
	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayThroughXYPlanes", &AABBoxCollissionTest::rayThroughXYPlanes));

	suite->addTest(new CppUnit::TestCaller<AABBoxCollissionTest>("rayInsideToXPlane", &AABBoxCollissionTest::rayInsideToXPlane));

	return suite;
}
