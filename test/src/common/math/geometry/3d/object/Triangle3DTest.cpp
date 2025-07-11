#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/object/Triangle3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Triangle3DTest::closestPoint() {
    std::array<float, 3> barycentrics{};
    Triangle3D triangle(Point3(1.0f, 1.0f, 0.0f), Point3(3.0f, 1.0f, 0.0f), Point3(2.0f, 3.0f, 0.0f));

    //point in front of triangle
    Point3<float> closestPoint1 = triangle.closestPoint(Point3(2.0f, 2.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint1, Point3(2.0f, 2.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.25f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.5f);

    //point in voronoi region of A
    Point3<float> closestPoint2 = triangle.closestPoint(Point3(0.0f, 0.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint2, Point3(1.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of B
    Point3<float> closestPoint3 = triangle.closestPoint(Point3(4.0f, 0.0f, 0.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint3, Point3(3.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of C
    Point3<float> closestPoint4 = triangle.closestPoint(Point3(2.1f, 4.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint4, Point3(2.0f, 3.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 1.0f);

    //point in voronoi region of AB
    Point3<float> closestPoint5 = triangle.closestPoint(Point3(2.0f, 0.0f, 5.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint5, Point3(2.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.0f);

    //point in voronoi region of AC
    Point3<float> closestPoint6 = triangle.closestPoint(Point3(0.0f, 2.0f, 1.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint6, Point3(1.2f, 1.4f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.8f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.2f);

    //point in voronoi region of BC
    Point3<float> closestPoint7 = triangle.closestPoint(Point3(3.1f, 2.0f, 5.0f), barycentrics);
    AssertHelper::assertPoint3FloatEquals(closestPoint7, Point3(2.62f, 1.76f, 0.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.62f);
    AssertHelper::assertFloatEquals(barycentrics[2], 0.38f);
}

void Triangle3DTest::projectedPointInsideTriangle() {
    Triangle3D triangle(Point3(1.0f, 1.0f, 0.0f), Point3(3.0f, 1.0f, 0.0f), Point3(2.0f, 3.0f, 0.0f));

    AssertHelper::assertTrue(triangle.projectedPointInsideTriangle(Point3(2.0f, 2.0f, 0.0f)));
    AssertHelper::assertTrue(triangle.projectedPointInsideTriangle(Point3(2.0f, 2.0f, -100.0f)));
    AssertHelper::assertTrue(triangle.projectedPointInsideTriangle(Point3(2.0f, 2.0f, 100.0f)));
    AssertHelper::assertFalse(triangle.projectedPointInsideTriangle(Point3(2.75f, 2.0f, 0.0f)));
    AssertHelper::assertFalse(triangle.projectedPointInsideTriangle(Point3(2.75f, 2.0f, -100.0f)));
    AssertHelper::assertFalse(triangle.projectedPointInsideTriangle(Point3(2.75f, 2.0f, 100.0f)));
}

void Triangle3DTest::noRayIntersectionParallel() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(-10.0f, -5.0f, 0.0f), Point3(10.0f, -5.0f, 0.0f));

    bool hasIntersection = false;
    triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertFalse(hasIntersection);
}

void Triangle3DTest::noRayIntersectionPerpendicular() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(1.0f, 1.5f, -0.2f), Point3(1.0f, -500.0f, -0.2f));

    bool hasIntersection = false;
    triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertFalse(hasIntersection);
}

void Triangle3DTest::noRayIntersectionOblique() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(-2.0f, 2.0f, -0.5f), Point3(60.0f, -0.5f, -0.5f));

    bool hasIntersection = false;
    triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertFalse(hasIntersection);
}

void Triangle3DTest::intersectionBeforeRay() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(1.0f, -0.5f, 0.2f), Point3(1.0f, -500.0f, 0.2f));

    bool hasIntersection = false;
    triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertFalse(hasIntersection);
}

void Triangle3DTest::intersectionAfterRay() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(1.0f, 500.0f, 0.2f), Point3(1.0f, 0.5f, 0.2f));

    bool hasIntersection = false;
    triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertFalse(hasIntersection);
}

void Triangle3DTest::rayIntersectionParallel() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(2.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 1.0f));
    Ray ray(Point3(1.0f, 1.5f, 0.2f), Point3(1.0f, -500.0f, 0.2f));

    bool hasIntersection = false;
    Point3 intersectionPoint = triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(1.0f, 0.0f, 0.2f));
}

void Triangle3DTest::rayIntersectionOblique() {
    Triangle3D triangle(Point3(0.0f, 0.0f, 0.0f), Point3(0.0f, 0.0f, -4.0f), Point3(1.0f, 5.0f, -0.5f));
    Ray ray(Point3(-3.0f, 3.0f, -2.2f), Point3(4.0f, 0.5f, -2.0f));

    bool hasIntersection = false;
    Point3 intersectionPoint = triangle.intersectPoint(ray, hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.36f, 1.8f, 2.104f));
}

CppUnit::Test* Triangle3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Triangle3DTest");

    suite->addTest(new CppUnit::TestCaller("closestPoint", &Triangle3DTest::closestPoint));
    suite->addTest(new CppUnit::TestCaller("projectedPointInsideTriangle", &Triangle3DTest::projectedPointInsideTriangle));

    suite->addTest(new CppUnit::TestCaller("noRayIntersectionParallel", &Triangle3DTest::noRayIntersectionParallel));
    suite->addTest(new CppUnit::TestCaller("noRayIntersectionPerpendicular", &Triangle3DTest::noRayIntersectionPerpendicular));
    suite->addTest(new CppUnit::TestCaller("noRayIntersectionOblique", &Triangle3DTest::noRayIntersectionOblique));
    suite->addTest(new CppUnit::TestCaller("intersectionBeforeRay", &Triangle3DTest::intersectionBeforeRay));
    suite->addTest(new CppUnit::TestCaller("intersectionAfterRay", &Triangle3DTest::intersectionAfterRay));
    suite->addTest(new CppUnit::TestCaller("rayIntersectionParallel", &Triangle3DTest::rayIntersectionParallel));
    suite->addTest(new CppUnit::TestCaller("rayIntersectionOblique", &Triangle3DTest::rayIntersectionOblique));

    return suite;
}
