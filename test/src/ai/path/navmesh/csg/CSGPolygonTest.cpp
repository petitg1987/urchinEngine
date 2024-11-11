#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/navmesh/csg/CSGPolygonTest.h>
#include <AssertHelper.h>
using namespace urchin;

void CSGPolygonTest::simplifySquare() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(1.0f, 0.1f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 4);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void CSGPolygonTest::simplifyMergeTwoNearPoints() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.05f, 1.0f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(1.0f, 0.1f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void CSGPolygonTest::simplifyWithThreeNearPoints() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.06f, 1.0f), Point2<float>(1.10f, 1.0f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(1.0f, 0.1f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 4);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.10f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void CSGPolygonTest::simplifyMergeStartEndPoints() {
    CSGPolygon<float> polygon("p1", {Point2<float>(1.05f, 1.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f)});

    polygon.simplify(1.0f, 0.1f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f)});
}

void CSGPolygonTest::simplifyFlatTriangle1() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, -0.02f)});

    polygon.simplify(0.99619469483f /*5 degrees*/, 0.0f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 0);
}

void CSGPolygonTest::simplifyFlatTriangle2() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, -0.02f), Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(0.99619469483f /*5 degrees*/, 0.0f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 0);
}

void CSGPolygonTest::simplifyUselessPoint() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.5f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(0.99619469483f /*5 degrees*/, 0.0f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void CSGPolygonTest::simplifyTwoUselessPoints() {
    CSGPolygon<float> polygon("p1", {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.5f), Point2<float>(1.0f, 0.3f), Point2<float>(1.0f, 0.0f)});

    polygon.simplify(0.99619469483f /*5 degrees*/, 0.0f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygon.getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void CSGPolygonTest::simplifyCorridor() { //see csgPolygonSimplifyCorridor.ggb
    CSGPolygon<float> polygon("p1", {
        Point2<float>(3.48144531f, -8.87060547f),
        Point2<float>(2.90893555f, -7.67126465f), //point 2 is very close to point 5
        Point2<float>(1.94140625f, -8.80053711f),
        Point2<float>(2.25842285f, -7.203125f),
        Point2<float>(2.90893555f, -7.67102051f), //point 5
        Point2<float>(2.55004883f, -6.91918945f),
        Point2<float>(4.14001465f, -6.14477539f)});

    polygon.simplify(1.0f, 0.01f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 7);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[1], Point2<float>(2.91324353f, -7.68028927f), 0.0001f);
}

void CSGPolygonTest::simplifyCorridorWithClosePoints() { //see csgPolygonSimplifyCorridorWithClosePoints.ggb
    float distanceThreshold = 0.01f;
    CSGPolygon<float> polygon("p1", {
            Point2<float>(3.48144531f, -8.87060547f),
            Point2<float>(2.90893555f, -7.67126465f - (distanceThreshold + 0.005f)), //point 1 is close to point 2 but not enough to be directly eliminated by the distance threshold
            Point2<float>(2.90893555f, -7.67126465f), //point 2 is very close to point 5
            Point2<float>(1.94140625f, -8.80053711f),
            Point2<float>(2.25842285f, -7.203125f),
            Point2<float>(2.90893555f, -7.67102051f), //point 5
            Point2<float>(2.55004883f, -6.91918945f),
            Point2<float>(4.14001465f, -6.14477539f)});

    polygon.simplify(1.0f, distanceThreshold);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 7);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[0], Point2<float>(3.48144531f, -8.87060547f), 0.0001f);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[1], Point2<float>(2.90893555f, -7.67126465f - (distanceThreshold + 0.005f)), 0.0001f);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[2], Point2<float>(1.94140625f, -8.80053711f), 0.0001f);
}

void CSGPolygonTest::simplifyFlatAngleAndNearPoints() { //see csgPolygonSimplifyFlatAngleAndNearPoints.ggb
    CSGPolygon<float> polygon("p1", {
            Point2<float>(27.8000488f, -19.4000244f), //close to last point
            Point2<float>(27.8000488f, -0.600097656f),
            Point2<float>(46.2000732f, -0.600097656f),
            Point2<float>(28.6002197f, -19.0002441f),
            Point2<float>(27.8001709f, -19.0002441f), //thisPoint + nextPoint + nextNextPoint forms a small angle
            Point2<float>(27.8001709f, -19.4000244f)});

    polygon.simplify(0.99619469483f /*5 degrees*/, 0.01f);

    AssertHelper::assertUnsignedIntEquals(polygon.getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[0], Point2<float>(27.8000488f, -0.600097656f));
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[1], Point2<float>(46.2000732f, -0.600097656f));
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[2], Point2<float>(28.6002197f, -19.0002441f));
    AssertHelper::assertPoint2FloatEquals(polygon.getCwPoints()[3], Point2<float>(27.8001709f, -19.0002441f));
}

CppUnit::Test* CSGPolygonTest::suite() {
    auto* suite = new CppUnit::TestSuite("CSGPolygonTest");

    suite->addTest(new CppUnit::TestCaller("simplifySquare", &CSGPolygonTest::simplifySquare));
    suite->addTest(new CppUnit::TestCaller("simplifyMergeTwoNearPoints", &CSGPolygonTest::simplifyMergeTwoNearPoints));
    suite->addTest(new CppUnit::TestCaller("simplifyWithThreeNearPoints", &CSGPolygonTest::simplifyWithThreeNearPoints));
    suite->addTest(new CppUnit::TestCaller("simplifyMergeStartEndPoints", &CSGPolygonTest::simplifyMergeStartEndPoints));
    suite->addTest(new CppUnit::TestCaller("simplifyFlatTriangle1", &CSGPolygonTest::simplifyFlatTriangle1));
    suite->addTest(new CppUnit::TestCaller("simplifyFlatTriangle2", &CSGPolygonTest::simplifyFlatTriangle2));
    suite->addTest(new CppUnit::TestCaller("simplifyUselessPoint", &CSGPolygonTest::simplifyUselessPoint));
    suite->addTest(new CppUnit::TestCaller("simplifyTwoUselessPoints", &CSGPolygonTest::simplifyTwoUselessPoints));
    suite->addTest(new CppUnit::TestCaller("simplifyCorridor", &CSGPolygonTest::simplifyCorridor));
    suite->addTest(new CppUnit::TestCaller("simplifyCorridorWithClosePoints", &CSGPolygonTest::simplifyCorridorWithClosePoints));
    suite->addTest(new CppUnit::TestCaller("simplifyFlatAngleAndNearPoints", &CSGPolygonTest::simplifyFlatAngleAndNearPoints));

    return suite;
}
