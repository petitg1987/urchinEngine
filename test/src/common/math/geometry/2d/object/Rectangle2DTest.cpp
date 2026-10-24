#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/math/geometry/2d/object/Rectangle2DTest.h"
#include "AssertHelper.h"
using namespace urchin;

void Rectangle2DTest::noOverlap() {
    Rectangle2D rectangle1(Point2(0.0f, 0.0f), Point2(1.0f, 1.0f));
    Rectangle2D rectangle2(Point2(2.0f, 0.0f), Point2(3.0f, 1.0f));

    AssertHelper::assertFloatEquals(rectangle1.computeOverlapArea(rectangle2), 0.0f);
    AssertHelper::assertFloatEquals(rectangle2.computeOverlapArea(rectangle1), 0.0f);
}

void Rectangle2DTest::touchingNoOverlap() {
    Rectangle2D rectangle1(Point2(0.0f, 0.0f), Point2(1.0f, 1.0f));
    Rectangle2D rectangle2(Point2(1.0f, 0.0f), Point2(2.0f, 1.0f));

    AssertHelper::assertFloatEquals(rectangle1.computeOverlapArea(rectangle2), 0.0f);
}

void Rectangle2DTest::partialOverlap() {
    Rectangle2D rectangle1(Point2(0.0f, 0.0f), Point2(2.0f, 2.0f));
    Rectangle2D rectangle2(Point2(1.0f, 1.5f), Point2(4.0f, 3.0f));

    AssertHelper::assertFloatEquals(rectangle1.computeOverlapArea(rectangle2), 0.5f);
    AssertHelper::assertFloatEquals(rectangle2.computeOverlapArea(rectangle1), 0.5f);
}

void Rectangle2DTest::fullOverlap() {
    Rectangle2D rectangle1(Point2(0.0f, 0.0f), Point2(4.0f, 4.0f));
    Rectangle2D rectangle2(Point2(1.0f, 1.0f), Point2(2.0f, 3.0f));

    AssertHelper::assertFloatEquals(rectangle1.computeOverlapArea(rectangle2), 2.0f);
    AssertHelper::assertFloatEquals(rectangle2.computeOverlapArea(rectangle1), 2.0f);
}

CppUnit::Test* Rectangle2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("Rectangle2DTest");

    suite->addTest(new CppUnit::TestCaller("noOverlap", &Rectangle2DTest::noOverlap));
    suite->addTest(new CppUnit::TestCaller("touchingNoOverlap", &Rectangle2DTest::touchingNoOverlap));
    suite->addTest(new CppUnit::TestCaller("partialOverlap", &Rectangle2DTest::partialOverlap));
    suite->addTest(new CppUnit::TestCaller("fullOverlap", &Rectangle2DTest::fullOverlap));

    return suite;
}
