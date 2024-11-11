#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/2d/shape/ConvexHullShape2DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void ConvexHullShape2DTest::convexHullBox() {
    std::vector obbPoints = {Point2(1.0f, 0.0f), Point2(0.0f, -1.0f), Point2(-1.0f, 0.0f), Point2(0.0f, 1.0f), Point2(0.0f, 0.0f) /*excluded points */};
    ConvexHullShape2D convexHull(obbPoints);

    AssertHelper::assertUnsignedIntEquals(convexHull.getPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[0], Point2(-1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[1], Point2(0.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[2], Point2(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[3], Point2(0.0f, 1.0f));
}

void ConvexHullShape2DTest::identicalPoints() {
    std::vector obbPoints = {Point2(1.0f, 0.0f), Point2(0.0f, -1.0f), Point2(-1.0f, 0.0f), Point2(0.0f, 1.0f),
            Point2(1.0f, 0.0f), Point2(0.0f, -1.0f), Point2(-1.0f, 0.0f) /* duplicate points */};
    ConvexHullShape2D convexHull(obbPoints);

    AssertHelper::assertUnsignedIntEquals(convexHull.getPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[0], Point2(-1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[1], Point2(0.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[2], Point2(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(convexHull.getPoints()[3], Point2(0.0f, 1.0f));
}

CppUnit::Test* ConvexHullShape2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("ConvexHullShape2DTest");

    suite->addTest(new CppUnit::TestCaller("convexHullBox", &ConvexHullShape2DTest::convexHullBox));
    suite->addTest(new CppUnit::TestCaller("identicalPoints", &ConvexHullShape2DTest::identicalPoints));

    return suite;
}
