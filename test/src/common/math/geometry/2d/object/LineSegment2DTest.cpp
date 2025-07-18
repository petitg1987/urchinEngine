#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/2d/object/LineSegment2DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LineSegment2DTest::closestPoint() {
    std::array<float, 2> barycentrics{};
    LineSegment2D lineSegment(Point2(1.0f, 1.0f), Point2(2.0f, 1.0f));

    //point outside segment (right)
    Point2<float> closestPoint1 = lineSegment.closestPoint(Point2(3.0f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint1, Point2(2.0f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0f);

    //point in segment
    Point2<float> closestPoint2 = lineSegment.closestPoint(Point2(1.5f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint2, Point2(1.5f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);

    //point outside segment (left)
    Point2<float> closestPoint3 = lineSegment.closestPoint(Point2(-3.0f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint3, Point2(1.0f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
}

void LineSegment2DTest::noIntersection() {
    LineSegment2D firstLine(Point2(-2.0, 0.0), Point2(-1.0, 0.0));
    LineSegment2D secondLine(Point2(0.0, 1.0), Point2(0.0, -1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertFalse(hasIntersection);
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::parallelLinesIntersection() {
    LineSegment2D firstLine(Point2(-2.0, 0.0), Point2(-1.0, 0.0));
    LineSegment2D secondLine(Point2(-2.0, 1.0), Point2(-1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertFalse(hasIntersection);
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersection() {
    LineSegment2D firstLine(Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f));
    LineSegment2D secondLine(Point2(-1.0f, 0.0f), Point2(1.0f, 0.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(0.0f, 0.0f));
    AssertHelper::assertTrue(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersectionOnOnePoint() {
    LineSegment2D firstLine(Point2(0.0f, 0.0f), Point2(0.0f, 1.0f));
    LineSegment2D secondLine(Point2(0.0f, -1.0f), Point2(0.0f, 0.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(0.0f, 0.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersectionOnOnePointInt() {
    LineSegment2D firstLine(Point2(-180, -4786), Point2(-180, -6068));
    LineSegment2D secondLine(Point2(-180, -6063), Point2(1096, -6063));

    bool hasIntersection;
    Point2<int> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2IntEquals(intersectionPoint, Point2(-180, -6062));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearIntersectionOnOnePoint() {
    LineSegment2D firstLine(Point2(-1.0f, 0.0f), Point2(0.0f, 0.0f));
    LineSegment2D secondLine(Point2(0.0f, 0.0f), Point2(0.0f, 1.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(0.0f, 0.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesNoIntersectionSameDirection() {
    LineSegment2D firstLine(Point2(-1.0, -1.0), Point2(1.0, 1.0));
    LineSegment2D secondLine(Point2(3.0, 3.0), Point2(4.0, 4.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertFalse(hasIntersection);
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesNoIntersectionOppositeDirection() {
    LineSegment2D firstLine(Point2(2.5, 2.5), Point2(1.5, 1.5));
    LineSegment2D secondLine(Point2(-1.25, -1.25), Point2(1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertFalse(hasIntersection);
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionSameDirection() {
    LineSegment2D firstLine(Point2(-1.0f, -1.0f), Point2(1.0f, 1.0f));
    LineSegment2D secondLine(Point2(0.0f, 0.0f), Point2(3.0f, 3.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(0.0f, 0.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionSameDirection2() {
    LineSegment2D firstLine(Point2(1.0f, 1.0f), Point2(4.0f, 4.0f));
    LineSegment2D secondLine(Point2(0.0f, 0.0f), Point2(3.0f, 3.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(1.0f, 1.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionOppositeDirection() {
    LineSegment2D firstLine(Point2(-1.25f, -1.25f), Point2(1.0f, 1.0f));
    LineSegment2D secondLine(Point2(0.0f, 0.0f), Point2(-2.5f, -2.5f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(-1.25f, -1.25f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionOppositeDirection2() {
    LineSegment2D firstLine(Point2(-3.0f, -3.0f), Point2(-1.25f, -1.25f));
    LineSegment2D secondLine(Point2(0.0f, 0.0f), Point2(-2.5f, -2.5f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(-2.5f, -2.5f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesFullIntersectionSameDirection() {
    LineSegment2D firstLine(Point2(-2.0f, 0.0f), Point2(2.0f, 0.0f));
    LineSegment2D secondLine(Point2(-1.5f, 0.0f), Point2(1.0f, 0.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(-1.5f, 0.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesFullIntersectionOppositeDirection() {
    LineSegment2D firstLine(Point2(-2.0f, 0.0f), Point2(2.0f, 0.0f));
    LineSegment2D secondLine(Point2(1.0f, 0.0f), Point2(-1.5f, 0.0f));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2(-1.5f, 0.0f));
    AssertHelper::assertFalse(firstLine.hasProperIntersection(secondLine));
}

CppUnit::Test* LineSegment2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("LineSegment2DTest");

    suite->addTest(new CppUnit::TestCaller("closestPoint", &LineSegment2DTest::closestPoint));

    suite->addTest(new CppUnit::TestCaller("noIntersection", &LineSegment2DTest::noIntersection));
    suite->addTest(new CppUnit::TestCaller("parallelLinesIntersection", &LineSegment2DTest::parallelLinesIntersection));
    suite->addTest(new CppUnit::TestCaller("intersection", &LineSegment2DTest::intersection));

    suite->addTest(new CppUnit::TestCaller("intersectionOnOnePoint", &LineSegment2DTest::intersectionOnOnePoint));
    suite->addTest(new CppUnit::TestCaller("intersectionOnOnePointInt", &LineSegment2DTest::intersectionOnOnePointInt));
    suite->addTest(new CppUnit::TestCaller("collinearIntersectionOnOnePoint", &LineSegment2DTest::collinearIntersectionOnOnePoint));

    suite->addTest(new CppUnit::TestCaller("collinearLinesNoIntersectionSameDirection", &LineSegment2DTest::collinearLinesNoIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller("collinearLinesNoIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesNoIntersectionOppositeDirection));

    suite->addTest(new CppUnit::TestCaller("collinearLinesIntersectionSameDirection", &LineSegment2DTest::collinearLinesIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller("collinearLinesIntersectionSameDirection2", &LineSegment2DTest::collinearLinesIntersectionSameDirection2));
    suite->addTest(new CppUnit::TestCaller("collinearLinesIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesIntersectionOppositeDirection));
    suite->addTest(new CppUnit::TestCaller("collinearLinesIntersectionOppositeDirection2", &LineSegment2DTest::collinearLinesIntersectionOppositeDirection2));
    suite->addTest(new CppUnit::TestCaller("collinearLinesFullIntersectionSameDirection", &LineSegment2DTest::collinearLinesFullIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller("collinearLinesFullIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesFullIntersectionOppositeDirection));

    return suite;
}
