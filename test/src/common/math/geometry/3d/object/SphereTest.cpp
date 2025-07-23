#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/math/geometry/3d/object/SphereTest.h"
#include "AssertHelper.h"
using namespace urchin;

void SphereTest::sphereLineIntersection() {
    Sphere sphere(1.5f, Point3(1.0f, 1.0f, 0.0f));
    Line3D line(Point3(-2.0f, 2.0f, 0.0f), Point3(4.0f, 2.0f, 0.0f));

    bool hasIntersection;
    Point3<float> firstCollision = sphere.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(firstCollision, Point3(-0.11803f, 2.0f, 0.0f));
}

void SphereTest::sphereAABBoxSideCollision() {
    Sphere sphere(1.0f, Point3(2.5f, 1.0f, 1.0f));
    AABBox aabBox(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(hasCollision);
}

void SphereTest::sphereAABBoxNoSideCollision() {
    Sphere sphere(1.0f, Point3(3.1f, 1.0f, 1.0f));
    AABBox aabBox(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertFalse(hasCollision);
}

void SphereTest::sphereAABBoxCornerCollision() {
    Sphere sphere(1.0f, Point3(2.6f, 2.6f, 1.0f));
    AABBox aabBox(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(hasCollision);
}

void SphereTest::sphereAABBoxNoCornerCollision() {
    Sphere sphere(1.0f, Point3(2.8f, 2.8f, 1.0f));
    AABBox aabBox(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertFalse(hasCollision);
}

CppUnit::Test* SphereTest::suite() {
    auto* suite = new CppUnit::TestSuite("SphereTest");

    suite->addTest(new CppUnit::TestCaller("sphereLineIntersection", &SphereTest::sphereLineIntersection));

    suite->addTest(new CppUnit::TestCaller("sphereAABBoxSideCollision", &SphereTest::sphereAABBoxSideCollision));
    suite->addTest(new CppUnit::TestCaller("sphereAABBoxNoSideCollision", &SphereTest::sphereAABBoxNoSideCollision));
    suite->addTest(new CppUnit::TestCaller("sphereAABBoxCornerCollision", &SphereTest::sphereAABBoxCornerCollision));
    suite->addTest(new CppUnit::TestCaller("sphereAABBoxNoCornerCollision", &SphereTest::sphereAABBoxNoCornerCollision));

    return suite;
}
