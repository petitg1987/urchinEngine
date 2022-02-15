#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/object/SphereTest.h>
#include <AssertHelper.h>
using namespace urchin;

void SphereTest::sphereLineIntersection() {
    Sphere<float> sphere(1.5f, Point3<float>(1.0f, 1.0f, 0.0f));
    Line3D<float> line(Point3<float>(-2.0f, 2.0f, 0.0f), Point3<float>(4.0f, 2.0f, 0.0f));

    bool hasIntersection;
    Point3<float> firstCollision = sphere.intersectPoint(line, hasIntersection);

    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(firstCollision, Point3<float>(-0.11803f, 2.0f, 0.0f));
}

void SphereTest::sphereAABBoxSideCollision() {
    Sphere<float> sphere(1.0f, Point3<float>(2.5f, 1.0f, 1.0f));
    AABBox<float> aabBox(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(hasCollision);
}

void SphereTest::sphereAABBoxNoSideCollision() {
    Sphere<float> sphere(1.0f, Point3<float>(3.1f, 1.0f, 1.0f));
    AABBox<float> aabBox(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(!hasCollision);
}

void SphereTest::sphereAABBoxCornerCollision() {
    Sphere<float> sphere(1.0f, Point3<float>(2.6f, 2.6f, 1.0f));
    AABBox<float> aabBox(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(hasCollision);
}

void SphereTest::sphereAABBoxNoCornerCollision() {
    Sphere<float> sphere(1.0f, Point3<float>(2.8f, 2.8f, 1.0f));
    AABBox<float> aabBox(Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(2.0f, 2.0f, 2.0f));

    bool hasCollision = sphere.collideWithAABBox(aabBox);

    AssertHelper::assertTrue(!hasCollision);
}

CppUnit::Test* SphereTest::suite() {
    auto* suite = new CppUnit::TestSuite("SphereTest");

    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereLineIntersection", &SphereTest::sphereLineIntersection));

    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereAABBoxSideCollision", &SphereTest::sphereAABBoxSideCollision));
    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereAABBoxNoSideCollision", &SphereTest::sphereAABBoxNoSideCollision));
    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereAABBoxCornerCollision", &SphereTest::sphereAABBoxCornerCollision));
    suite->addTest(new CppUnit::TestCaller<SphereTest>("sphereAABBoxNoCornerCollision", &SphereTest::sphereAABBoxNoCornerCollision));

    return suite;
}
