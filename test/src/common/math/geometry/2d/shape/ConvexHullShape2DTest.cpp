#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/2d/shape/ConvexHullShape2DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void ConvexHullShape2DTest::convexHullBox() {
    std::vector<Point2<float>> obbPoints = {Point2<float>(1.0, 0.0), Point2<float>(0.0, -1.0), Point2<float>(-1.0, 0.0), Point2<float>(0.0, 1.0),
            Point2<float>(0.0, 0.0) /*excluded points */};
    ConvexHullShape2D<float> convexHull(obbPoints);

    AssertHelper::assertUnsignedIntEquals(convexHull.getPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[0], Point2<float>(-1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[1], Point2<float>(0.0, -1.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[2], Point2<float>(1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[3], Point2<float>(0.0, 1.0));
}

void ConvexHullShape2DTest::identicalPoints() {
    std::vector<Point2<float>> obbPoints = {Point2<float>(1.0, 0.0), Point2<float>(0.0, -1.0), Point2<float>(-1.0, 0.0), Point2<float>(0.0, 1.0),
            Point2<float>(1.0, 0.0), Point2<float>(0.0, -1.0), Point2<float>(-1.0, 0.0) /* duplicate points */};
    ConvexHullShape2D<float> convexHull(obbPoints);

    AssertHelper::assertUnsignedIntEquals(convexHull.getPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[0], Point2<float>(-1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[1], Point2<float>(0.0, -1.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[2], Point2<float>(1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[3], Point2<float>(0.0, 1.0));
}

CppUnit::Test* ConvexHullShape2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("ConvexHullShape2DTest");

    suite->addTest(new CppUnit::TestCaller("convexHullBox", &ConvexHullShape2DTest::convexHullBox));
    suite->addTest(new CppUnit::TestCaller("identicalPoints", &ConvexHullShape2DTest::identicalPoints));

    return suite;
}
