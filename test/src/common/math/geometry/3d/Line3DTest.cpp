#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/Line3DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void Line3DTest::orthogonalProjection() {
    Line3D line(Point3(0.5f, 1.0f, 0.0f), Point3(2.0f, 1.0f, 0.0f));
    Point3<float> projectedPoint = line.orthogonalProjection(Point3(3.0f, 2.0f, 0.0f));

    AssertHelper::assertPoint3FloatEquals(projectedPoint, Point3(3.0f, 1.0f, 0.0f));
}

CppUnit::Test* Line3DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Line3DTest");

    suite->addTest(new CppUnit::TestCaller<Line3DTest>("orthogonalProjection", &Line3DTest::orthogonalProjection));

    return suite;
}
