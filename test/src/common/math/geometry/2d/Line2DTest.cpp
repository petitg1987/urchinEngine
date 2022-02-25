#include <cppunit/extensions/HelperMacros.h>

#include <common/math/geometry/2d/Line2DTest.h>
#include <AssertHelper.h>
using namespace urchin;


void Line2DTest::orthogonalProjection() {
    Line2D line(Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 1.0f));
    Point2<float> projectedPoint = line.orthogonalProjection(Point2<float>(3.0f, 2.0f));

    AssertHelper::assertPoint2FloatEquals(projectedPoint, Point2<float>(3.0f, 1.0f));
}

CppUnit::Test* Line2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Line2DTest");

    suite->addTest(new CppUnit::TestCaller<Line2DTest>("orthogonalProjection", &Line2DTest::orthogonalProjection));

    return suite;
}
