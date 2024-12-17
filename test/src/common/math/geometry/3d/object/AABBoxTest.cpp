#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/AABBoxTest.h>
#include <AssertHelper.h>
using namespace urchin;

void AABBoxTest::rayRightToBox() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(2.0f, 0.5f, 2.0f), Vector3(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::assertFalse(box.collideWithRay(ray));
}

void AABBoxTest::rayRightTopToBox() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(1.5f, 2.0f, 0.5f), Vector3(1.0f, 1.0f, 0.0f), 10.0f);

    AssertHelper::assertFalse(box.collideWithRay(ray));
}

void AABBoxTest::rayInsideBox() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(0.1f, 0.1f, 0.1f), Vector3(1.0f, 1.0f, 1.0f), 1.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughXPlanes() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(-1.0f, 0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), 10.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughYPlanes() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(0.5f, 2.0f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), 2.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughZPlanes() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(0.5f, 0.5f, 2.0f), Vector3(0.0f, 0.0f, -1.0f), 10.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayThroughXYPlanes() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(1.5f, 2.0f, 0.5f), Vector3(-1.0f, -1.0f, 0.0f), 4.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

void AABBoxTest::rayInsideToXPlane() {
    AABBox box(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f));
    Ray ray(Point3(0.5f, 0.5f, 0.5f), Vector3(1.0f, 0.0f, 0.0f), 5.0f);

    AssertHelper::assertTrue(box.collideWithRay(ray));
}

CppUnit::Test* AABBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("AABBoxTest");

    suite->addTest(new CppUnit::TestCaller("rayRightToBox", &AABBoxTest::rayRightToBox));
    suite->addTest(new CppUnit::TestCaller("rayRightTopToBox", &AABBoxTest::rayRightTopToBox));

    suite->addTest(new CppUnit::TestCaller("rayInsideBox", &AABBoxTest::rayInsideBox));

    suite->addTest(new CppUnit::TestCaller("rayThroughXPlanes", &AABBoxTest::rayThroughXPlanes));
    suite->addTest(new CppUnit::TestCaller("rayThroughYPlanes", &AABBoxTest::rayThroughYPlanes));
    suite->addTest(new CppUnit::TestCaller("rayThroughZPlanes", &AABBoxTest::rayThroughZPlanes));
    suite->addTest(new CppUnit::TestCaller("rayThroughXYPlanes", &AABBoxTest::rayThroughXYPlanes));

    suite->addTest(new CppUnit::TestCaller("rayInsideToXPlane", &AABBoxTest::rayInsideToXPlane));

    return suite;
}
