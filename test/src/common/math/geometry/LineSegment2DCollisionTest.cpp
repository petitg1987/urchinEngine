#include <cppunit/extensions/HelperMacros.h>
#include "UrchinCommon.h"

#include "common/math/geometry/LineSegment2DCollisionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void LineSegment2DCollisionTest::noIntersection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(-1.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 1.0), Point2<float>(0.0, -1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::parallelLines() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(-1.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(-2.0, 1.0), Point2<float>(-1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::intersection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(-1.0, 0.0), Point2<float>(1.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::intersectionOnOnePoint() {
    LineSegment2D<float> firstLine(Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, -1.0), Point2<float>(0.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::intersectionOnOnePointInt() {
    LineSegment2D<int> firstLine(Point2<int>(-180, -4786), Point2<int>(-180, -6068));
    LineSegment2D<int> secondLine(Point2<int>(-180, -6063), Point2<int>(1096, -6063));

    bool hasIntersection;
    Point2<int> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2IntEquals(intersectionPoint, Point2<int>(-180, -6062));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearIntersectionOnOnePoint() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, 0.0), Point2<float>(0.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesNoIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(3.0, 3.0), Point2<float>(4.0, 4.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesNoIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(2.5, 2.5), Point2<float>(1.5, 1.5));
    LineSegment2D<float> secondLine(Point2<float>(-1.25, -1.25), Point2<float>(1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(3.0, 3.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesIntersectionSameDirection2() {
    LineSegment2D<float> firstLine(Point2<float>(1.0, 1.0), Point2<float>(4.0, 4.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(3.0, 3.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(1.0, 1.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.25, -1.25), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(-2.5, -2.5));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.25, -1.25));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesIntersectionOppositeDirection2() {
    LineSegment2D<float> firstLine(Point2<float>(-3.0, -3.0), Point2<float>(-1.25, -1.25));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(-2.5, -2.5));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-2.5, -2.5));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesFullIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(2.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(-1.5, 0.0), Point2<float>(1.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.5, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DCollisionTest::collinearLinesFullIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(2.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(1.0, 0.0), Point2<float>(-1.5, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.5, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

CppUnit::Test *LineSegment2DCollisionTest::suite() {
    auto* suite = new CppUnit::TestSuite("LineSegment2DCollisionTest");

    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("noIntersection", &LineSegment2DCollisionTest::noIntersection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("parallelLines", &LineSegment2DCollisionTest::parallelLines));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("intersection", &LineSegment2DCollisionTest::intersection));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("intersectionOnOnePoint", &LineSegment2DCollisionTest::intersectionOnOnePoint));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("intersectionOnOnePointInt", &LineSegment2DCollisionTest::intersectionOnOnePointInt));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearIntersectionOnOnePoint", &LineSegment2DCollisionTest::collinearIntersectionOnOnePoint));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesNoIntersectionSameDirection", &LineSegment2DCollisionTest::collinearLinesNoIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesNoIntersectionOppositeDirection", &LineSegment2DCollisionTest::collinearLinesNoIntersectionOppositeDirection));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesIntersectionSameDirection", &LineSegment2DCollisionTest::collinearLinesIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesIntersectionSameDirection2", &LineSegment2DCollisionTest::collinearLinesIntersectionSameDirection2));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesIntersectionOppositeDirection", &LineSegment2DCollisionTest::collinearLinesIntersectionOppositeDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesIntersectionOppositeDirection2", &LineSegment2DCollisionTest::collinearLinesIntersectionOppositeDirection2));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesFullIntersectionSameDirection", &LineSegment2DCollisionTest::collinearLinesFullIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DCollisionTest>("collinearLinesFullIntersectionOppositeDirection", &LineSegment2DCollisionTest::collinearLinesFullIntersectionOppositeDirection));

    return suite;
}
