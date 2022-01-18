#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include <common/math/geometry/2d/object/LineSegment2DTest.h>
#include <AssertHelper.h>
using namespace urchin;

void LineSegment2DTest::closestPoint() {
    std::array<float, 2> barycentrics{};
    LineSegment2D<float> lineSegment(Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 1.0f));

    //point outside segment (right)
    Point2<float> closestPoint1 = lineSegment.closestPoint(Point2<float>(3.0f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint1, Point2<float>(2.0f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 1.0f);

    //point in segment
    Point2<float> closestPoint2 = lineSegment.closestPoint(Point2<float>(1.5f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint2, Point2<float>(1.5f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 0.5f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.5f);

    //point outside segment (left)
    Point2<float> closestPoint3 = lineSegment.closestPoint(Point2<float>(-3.0f, 2.0f), barycentrics);
    AssertHelper::assertPoint2FloatEquals(closestPoint3, Point2<float>(1.0f, 1.0f));
    AssertHelper::assertFloatEquals(barycentrics[0], 1.0f);
    AssertHelper::assertFloatEquals(barycentrics[1], 0.0f);
}

void LineSegment2DTest::noIntersection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(-1.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 1.0), Point2<float>(0.0, -1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::parallelLines() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(-1.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(-2.0, 1.0), Point2<float>(-1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(-1.0, 0.0), Point2<float>(1.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersectionOnOnePoint() {
    LineSegment2D<float> firstLine(Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, -1.0), Point2<float>(0.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::intersectionOnOnePointInt() {
    LineSegment2D<int> firstLine(Point2<int>(-180, -4786), Point2<int>(-180, -6068));
    LineSegment2D<int> secondLine(Point2<int>(-180, -6063), Point2<int>(1096, -6063));

    bool hasIntersection;
    Point2<int> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2IntEquals(intersectionPoint, Point2<int>(-180, -6062));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearIntersectionOnOnePoint() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, 0.0), Point2<float>(0.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesNoIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(3.0, 3.0), Point2<float>(4.0, 4.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesNoIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(2.5, 2.5), Point2<float>(1.5, 1.5));
    LineSegment2D<float> secondLine(Point2<float>(-1.25, -1.25), Point2<float>(1.0, 1.0));

    bool hasIntersection;
    firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertTrue(!hasIntersection);
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.0, -1.0), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(3.0, 3.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(0.0, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionSameDirection2() {
    LineSegment2D<float> firstLine(Point2<float>(1.0, 1.0), Point2<float>(4.0, 4.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(3.0, 3.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(1.0, 1.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-1.25, -1.25), Point2<float>(1.0, 1.0));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(-2.5, -2.5));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.25, -1.25));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesIntersectionOppositeDirection2() {
    LineSegment2D<float> firstLine(Point2<float>(-3.0, -3.0), Point2<float>(-1.25, -1.25));
    LineSegment2D<float> secondLine(Point2<float>(0.0, 0.0), Point2<float>(-2.5, -2.5));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-2.5, -2.5));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesFullIntersectionSameDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(2.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(-1.5, 0.0), Point2<float>(1.0, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.5, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

void LineSegment2DTest::collinearLinesFullIntersectionOppositeDirection() {
    LineSegment2D<float> firstLine(Point2<float>(-2.0, 0.0), Point2<float>(2.0, 0.0));
    LineSegment2D<float> secondLine(Point2<float>(1.0, 0.0), Point2<float>(-1.5, 0.0));

    bool hasIntersection;
    Point2<float> intersectionPoint = firstLine.intersectPoint(secondLine, hasIntersection);

    AssertHelper::assertPoint2FloatEquals(intersectionPoint, Point2<float>(-1.5, 0.0));
    AssertHelper::assertTrue(!firstLine.hasProperIntersection(secondLine));
}

CppUnit::Test* LineSegment2DTest::suite() {
    auto* suite = new CppUnit::TestSuite("LineSegment2DTest");

    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("closestPoint", &LineSegment2DTest::closestPoint));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("noIntersection", &LineSegment2DTest::noIntersection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("parallelLines", &LineSegment2DTest::parallelLines));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("intersection", &LineSegment2DTest::intersection));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("intersectionOnOnePoint", &LineSegment2DTest::intersectionOnOnePoint));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("intersectionOnOnePointInt", &LineSegment2DTest::intersectionOnOnePointInt));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearIntersectionOnOnePoint", &LineSegment2DTest::collinearIntersectionOnOnePoint));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesNoIntersectionSameDirection", &LineSegment2DTest::collinearLinesNoIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesNoIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesNoIntersectionOppositeDirection));

    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesIntersectionSameDirection", &LineSegment2DTest::collinearLinesIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesIntersectionSameDirection2", &LineSegment2DTest::collinearLinesIntersectionSameDirection2));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesIntersectionOppositeDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesIntersectionOppositeDirection2", &LineSegment2DTest::collinearLinesIntersectionOppositeDirection2));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesFullIntersectionSameDirection", &LineSegment2DTest::collinearLinesFullIntersectionSameDirection));
    suite->addTest(new CppUnit::TestCaller<LineSegment2DTest>("collinearLinesFullIntersectionOppositeDirection", &LineSegment2DTest::collinearLinesFullIntersectionOppositeDirection));

    return suite;
}
