#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/PlaneTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PlaneTest::orthogonalProjection() {
    //test 1
    Plane plane1(Point3(1.0f, 0.0f, 0.0f), Point3(1.0f, 5.0f, 1.0f), Point3(1.0f, 10.0f, 0.0f));
    Point3<float> projectedPoint1 = plane1.orthogonalProjection(Point3(-3.0f, -3.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(projectedPoint1, Point3(1.0f, -3.0f, 0.0f));

    //test 2
    Plane plane2(Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 1.0f, 1.0f), Point3(2.0f, 2.0f, 0.0f));
    Point3<float> projectedPoint2 = plane2.orthogonalProjection(Point3(0.0f, 2.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(projectedPoint2, Point3(1.0f, 1.0f, 0.0f));
}

void PlaneTest::intersectPoint() {
    Plane plane(Point3(-1.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(0.0f, 5.0f, 0.0f));

    //test 1
    bool hasIntersection = false;
    Point3<float> intersectionPoint = plane.intersectPoint(Line3D(Point3(0.0f, 1.0f, 5.0f), Point3(0.0f, 1.0f, 10.0f)), hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.0f, 0.0f));

    //test 2
    hasIntersection = false;
    intersectionPoint = plane.intersectPoint(Line3D(Point3(0.0f, 1.0f, -5.0f), Point3(0.0f, 1.0f, -10.0f)), hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.0f, 0.0f));

    //test 3
    hasIntersection = false;
    plane.intersectPoint(LineSegment3D(Point3(0.0f, 1.0f, 5.0f), Point3(0.0f, 1.0f, 10.0f)), hasIntersection);
    AssertHelper::assertFalse(hasIntersection);

    //test 4
    hasIntersection = false;
    plane.intersectPoint(LineSegment3D(Point3(0.0f, 1.0f, -5.0f), Point3(0.0f, 1.0f, -10.0f)), hasIntersection);
    AssertHelper::assertTrue(!hasIntersection);

    //test 5
    hasIntersection = false;
    intersectionPoint = plane.intersectPoint(LineSegment3D(Point3(0.0f, 1.0f, -5.0f), Point3(0.0f, 1.0f, 5.0f)), hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.0f, 0.0f));

    //test 6
    hasIntersection = false;
    intersectionPoint = plane.intersectPoint(LineSegment3D(Point3(0.0f, 1.0f, 5.0f), Point3(0.0f, 1.0f, -5.0f)), hasIntersection);
    AssertHelper::assertTrue(hasIntersection);
    AssertHelper::assertPoint3FloatEquals(intersectionPoint, Point3(0.0f, 1.0f, 0.0f));
}

CppUnit::Test* PlaneTest::suite() {
    auto* suite = new CppUnit::TestSuite("PlaneTest");

    suite->addTest(new CppUnit::TestCaller("orthogonalProjection", &PlaneTest::orthogonalProjection));
    suite->addTest(new CppUnit::TestCaller("intersectPoint", &PlaneTest::intersectPoint));

    return suite;
}
