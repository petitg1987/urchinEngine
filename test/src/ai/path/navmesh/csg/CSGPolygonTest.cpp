#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "CSGPolygonTest.h"
#include "AssertHelper.h"
using namespace urchin;

void CSGPolygonTest::simplifySquare()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.0)});

    polygon.simplify(1.0, 0.1);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 4);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.0)});
}

void CSGPolygonTest::simplifyMergeTwoNearPoints()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.05, 1.0), Point2<float>(1.0, 0.0)});

    polygon.simplify(1.0, 0.1);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.0)});
}

void CSGPolygonTest::simplifyWithThreeNearPoints()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.06, 1.0), Point2<float>(1.10, 1.0), Point2<float>(1.0, 0.0)});

    polygon.simplify(1.0, 0.1);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 4);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.10, 1.0), Point2<float>(1.0, 0.0)});
}

void CSGPolygonTest::simplifyMergeStartEndPoints()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(1.05, 1.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0)});

    polygon.simplify(1.0, 0.1);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(1.0, 0.0), Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0)});
}

void CSGPolygonTest::simplifyFlatTriangle1()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, -0.02)});

    polygon.simplify(0.99619469483 /*5 degrees*/, 0.0);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 0);
}

void CSGPolygonTest::simplifyFlatTriangle2()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, -0.02), Point2<float>(0.0, 0.0), Point2<float>(1.0, 0.0)});

    polygon.simplify(0.99619469483 /*5 degrees*/, 0.0);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 0);
}

void CSGPolygonTest::simplifyUselessPoint()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.5), Point2<float>(1.0, 0.0)});

    polygon.simplify(0.99619469483 /*5 degrees*/, 0.0);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.0)});
}

void CSGPolygonTest::simplifyTwoUselessPoints()
{
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.5), Point2<float>(1.0, 0.3), Point2<float>(1.0, 0.0)});

    polygon.simplify(0.99619469483 /*5 degrees*/, 0.0);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(1.0, 0.0)});
}

void CSGPolygonTest::simplifyCorridor()
{ //see csgPolygonSimplifyCorridor.ggb
    CSGPolygon<float> polygon("p1", {
        Point2<float>(3.48144531, -8.87060547),
        Point2<float>(2.90893555, -7.67126465), //point 2 is very close to point 5
        Point2<float>(1.94140625, -8.80053711),
        Point2<float>(2.25842285, -7.203125),
        Point2<float>(2.90893555, -7.67102051), //point 5
        Point2<float>(2.55004883, -6.91918945),
        Point2<float>(4.14001465, -6.14477539)});

    polygon.simplify(1.0, 0.01);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 7);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[1], Point2<float>(2.91324353, -7.68028927), 0.0001);
}

void CSGPolygonTest::simplifyCorridorWithClosePoints()
{ //see csgPolygonSimplifyCorridorWithClosePoints.ggb
    float distanceThreshold = 0.01f;
    CSGPolygon<float> polygon("p1", {
            Point2<float>(3.48144531, -8.87060547),
            Point2<float>(2.90893555, -7.67126465 - (distanceThreshold + 0.005f)), //point 1 is close to point 2 but not enough to be directly eliminated by the distance threshold
            Point2<float>(2.90893555, -7.67126465), //point 2 is very close to point 5
            Point2<float>(1.94140625, -8.80053711),
            Point2<float>(2.25842285, -7.203125),
            Point2<float>(2.90893555, -7.67102051), //point 5
            Point2<float>(2.55004883, -6.91918945),
            Point2<float>(4.14001465, -6.14477539)});

    polygon.simplify(1.0, distanceThreshold);

    AssertHelper::assertUnsignedInt(polygon.getCwPoints().size(), 7);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[0], Point2<float>(3.48144531, -8.87060547), 0.0001);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[1], Point2<float>(2.90893555, -7.67126465 - (distanceThreshold + 0.005f)), 0.0001);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[2], Point2<float>(1.94140625, -8.80053711), 0.0001);
}

CppUnit::Test *CSGPolygonTest::suite()
{
    auto *suite = new CppUnit::TestSuite("CSGPolygonTest");

    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifySquare", &CSGPolygonTest::simplifySquare));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyMergeTwoNearPoints", &CSGPolygonTest::simplifyMergeTwoNearPoints));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyWithThreeNearPoints", &CSGPolygonTest::simplifyWithThreeNearPoints));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyMergeStartEndPoints", &CSGPolygonTest::simplifyMergeStartEndPoints));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyFlatTriangle1", &CSGPolygonTest::simplifyFlatTriangle1));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyFlatTriangle2", &CSGPolygonTest::simplifyFlatTriangle2));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyUselessPoint", &CSGPolygonTest::simplifyUselessPoint));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyTwoUselessPoints", &CSGPolygonTest::simplifyTwoUselessPoints));

    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyCorridor", &CSGPolygonTest::simplifyCorridor));
    suite->addTest(new CppUnit::TestCaller<CSGPolygonTest>("simplifyCorridorWithClosePoints", &CSGPolygonTest::simplifyCorridorWithClosePoints));

    return suite;
}
