#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/AABBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void AABBoxTest::rayRightToBox() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(2.0f, 0.5f, 2.0f), Vector3<float>(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::assertTrue(!box.collideWithRay(ray));
}

void AABBoxTest::rayRightTopToBox() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(1.5f, 2.0f, 0.5f), Vector3<float>(1.0f, 1.0f, 0.0f), 10.0f);

    AssertHelper::assertTrue(!box.collideWithRay(ray));
}

void AABBoxTest::rayInsideBox() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(0.1f, 0.1f, 0.1f), Vector3<float>(1.0f, 1.0f, 1.0f), 1.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughXPlanes() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(-1.0f, 0.5f, 0.5f), Vector3<float>(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughYPlanes() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(0.5f, 2.0f, 0.5f), Vector3<float>(0.0f, -1.0f, 0.0f), 2.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughZPlanes() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(0.5f, 0.5f, 2.0f), Vector3<float>(0.0f, 0.0f, -1.0f), 10.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughXYPlanes() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(1.5f, 2.0f, 0.5f), Vector3<float>(-1.0f, -1.0f, 0.0f), 4.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayInsideToXPlane() {
    AABBox box(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(1.0f, 1.0f, 1.0f));
    Ray ray(Point3<float>(0.5f, 0.5f, 0.5f), Vector3<float>(1.0f, 0.0f, 0.0f), 5.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

CppUnit::Test* AABBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("AABBoxTest");

    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayRightToBox", &AABBoxTest::rayRightToBox));
    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayRightTopToBox", &AABBoxTest::rayRightTopToBox));

    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayInsideBox", &AABBoxTest::rayInsideBox));

    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayThroughXPlanes", &AABBoxTest::rayThroughXPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayThroughYPlanes", &AABBoxTest::rayThroughYPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayThroughZPlanes", &AABBoxTest::rayThroughZPlanes));
    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayThroughXYPlanes", &AABBoxTest::rayThroughXYPlanes));

    suite->addTest(new CppUnit::TestCaller<AABBoxTest>("rayInsideToXPlane", &AABBoxTest::rayInsideToXPlane));

    return suite;
}
