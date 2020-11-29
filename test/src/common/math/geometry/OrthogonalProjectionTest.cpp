#include <cppunit/extensions/HelperMacros.h>

#include "common/math/geometry/OrthogonalProjectionTest.h"
#include "AssertHelper.h"
using namespace urchin;


void OrthogonalProjectionTest::orthogonalProjectionLine2D() {
    Line2D<float> line(Point2<float>(1.0, 1.0), Point2<float>(2.0, 1.0));
    Point2<float> projectedPoint = line.orthogonalProjection(Point2<float>(3.0, 2.0));

    AssertHelper::assertPoint2FloatEquals(projectedPoint, Point2<float>(3.0, 1.0));
}

void OrthogonalProjectionTest::orthogonalProjectionLine3D() {
    Line3D<float> line(Point3<float>(1.0, 1.0, 0.0), Point3<float>(2.0, 1.0, 0.0));
    Point3<float> projectedPoint = line.orthogonalProjection(Point3<float>(3.0, 2.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(projectedPoint, Point3<float>(3.0, 1.0, 0.0));
}

void OrthogonalProjectionTest::orthogonalProjectionPlane() {
    //test 1
    Plane<float> plane1(Point3<float>(1.0, 0.0, 0.0), Point3<float>(1.0, 5.0, 1.0), Point3<float>(1.0, 10.0, 0.0));
    Point3<float> projectedPoint1 = plane1.orthogonalProjection(Point3<float>(-3.0, -3.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(projectedPoint1, Point3<float>(1.0, -3.0, 0.0));

    //test 2
    Plane<float> plane2(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0), Point3<float>(2.0, 2.0, 0.0));
    Point3<float> projectedPoint2 = plane2.orthogonalProjection(Point3<float>(0.0, 2.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(projectedPoint2, Point3<float>(1.0, 1.0, 0.0));
}

CppUnit::Test *OrthogonalProjectionTest::suite() {
    auto* suite = new CppUnit::TestSuite("OrthogonalProjectionTest");

    suite->addTest(new CppUnit::TestCaller<OrthogonalProjectionTest>("orthogonalProjectionLine2D", &OrthogonalProjectionTest::orthogonalProjectionLine2D));
    suite->addTest(new CppUnit::TestCaller<OrthogonalProjectionTest>("orthogonalProjectionLine3D", &OrthogonalProjectionTest::orthogonalProjectionLine3D));

    suite->addTest(new CppUnit::TestCaller<OrthogonalProjectionTest>("orthogonalProjectionPlane", &OrthogonalProjectionTest::orthogonalProjectionPlane));

    return suite;
}
