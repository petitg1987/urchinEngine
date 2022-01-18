#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/3d/Line3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Line3DTest::orthogonalProjection() {
    Line3D<float> line(Point3<float>(1.0, 1.0, 0.0), Point3<float>(2.0, 1.0, 0.0));
    Point3<float> projectedPoint = line.orthogonalProjection(Point3<float>(3.0, 2.0, 0.0));

    AssertHelper::assertPoint3FloatEquals(projectedPoint, Point3<float>(3.0, 1.0, 0.0));
}

CppUnit::Test* Line3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Line3DTest");

    suite->addTest(new CppUnit::TestCaller<Line3DTest>("orthogonalProjection", &Line3DTest::orthogonalProjection));

    return suite;
}
