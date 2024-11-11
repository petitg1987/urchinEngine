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

CppUnit::Test* PlaneTest::suite() {
    auto* suite = new CppUnit::TestSuite("PlaneTest");

    suite->addTest(new CppUnit::TestCaller<PlaneTest>("orthogonalProjection", &PlaneTest::orthogonalProjection));

    return suite;
}
