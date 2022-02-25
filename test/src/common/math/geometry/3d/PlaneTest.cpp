#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/PlaneTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PlaneTest::orthogonalProjection() {
    //test 1
    Plane plane1(Point3<float>(1.0, 0.0, 0.0), Point3<float>(1.0, 5.0, 1.0), Point3<float>(1.0, 10.0, 0.0));
    Point3<float> projectedPoint1 = plane1.orthogonalProjection(Point3<float>(-3.0, -3.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(projectedPoint1, Point3<float>(1.0, -3.0, 0.0));

    //test 2
    Plane plane2(Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 1.0, 1.0), Point3<float>(2.0, 2.0, 0.0));
    Point3<float> projectedPoint2 = plane2.orthogonalProjection(Point3<float>(0.0, 2.0, 0.0));
    AssertHelper::assertPoint3FloatEquals(projectedPoint2, Point3<float>(1.0, 1.0, 0.0));
}

CppUnit::Test* PlaneTest::suite() {
    auto* suite = new CppUnit::TestSuite("PlaneTest");

    suite->addTest(new CppUnit::TestCaller<PlaneTest>("orthogonalProjection", &PlaneTest::orthogonalProjection));

    return suite;
}
