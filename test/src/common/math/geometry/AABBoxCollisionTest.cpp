#include <cppunit/extensions/HelperMacros.h>

#include "common/math/geometry/AABBoxCollisionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void AABBoxCollisionTest::rayRightToBox() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(2.0f, 0.5f, 2.0f), Vector3<float>(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::instance()->assertTrue(!box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayRightTopToBox() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(1.5f, 2.0f, 0.5f), Vector3<float>(1.0f, 1.0f, 0.0f), 10.0f);

    AssertHelper::instance()->assertTrue(!box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayInsideBox() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(0.1f, 0.1f, 0.1f), Vector3<float>(1.0f, 1.0f, 1.0f), 1.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayThroughXPlanes() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(-1.0f, 0.5f, 0.5f), Vector3<float>(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayThroughYPlanes() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(0.5f, 2.0f, 0.5f), Vector3<float>(0.0f, -1.0f, 0.0f), 2.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayThroughZPlanes() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(0.5f, 0.5f, 2.0f), Vector3<float>(0.0f, 0.0f, -1.0f), 10.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayThroughXYPlanes() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(1.5f, 2.0f, 0.5f), Vector3<float>(-1.0f, -1.0f, 0.0f), 4.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

void AABBoxCollisionTest::rayInsideToXPlane() {
    AABBox<float> box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray<float> ray(Point3<float>(0.5f, 0.5f, 0.5f), Vector3<float>(1.0f, 0.0f, 0.0f), 5.0f);

    AssertHelper::instance()->assertTrue(box.collideWithRay(ray));
}

CppUnit::Test* AABBoxCollisionTest::suite() {
    auto* suite = new CppUnit::TestSuite("AABBoxCollisionTest");

    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayRightToBox", &AABBoxCollisionTest::rayRightToBox));
    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayRightTopToBox", &AABBoxCollisionTest::rayRightTopToBox));

    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayInsideBox", &AABBoxCollisionTest::rayInsideBox));

    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayThroughXPlanes", &AABBoxCollisionTest::rayThroughXPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayThroughYPlanes", &AABBoxCollisionTest::rayThroughYPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayThroughZPlanes", &AABBoxCollisionTest::rayThroughZPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayThroughXYPlanes", &AABBoxCollisionTest::rayThroughXYPlanes));

    suite->addTest(new CppUnit::TestCaller<AABBoxCollisionTest>("rayInsideToXPlane", &AABBoxCollisionTest::rayInsideToXPlane));

    return suite;
}
