#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/navmesh/csg/PolygonsSubtractionTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 3.0f), Point2<float>(3.0f, 3.0f),
                                                 Point2<float>(3.0f, 1.0f), Point2<float>(1.0f, 1.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactPoint() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 3.0f), Point2<float>(4.0f, 3.0f),
                                                 Point2<float>(3.0f, 1.0f), Point2<float>(1.0f, 1.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactEdge() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 3.0f), Point2<float>(4.0f, 3.0f),
                                                 Point2<float>(4.0f, 1.0f), Point2<float>(1.0f, 1.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 3.0f),
                                                                                 Point2<float>(1.0f, 3.0f), Point2<float>(1.0f, 1.0f),
                                                                                 Point2<float>(4.0f, 1.0f), Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 7.0f), Point2<float>(3.0f, 7.0f),
                                                 Point2<float>(3.0f, 5.0f), Point2<float>(1.0f, 5.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionMinuendInside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0f, 3.0f), Point2<float>(3.0f, 3.0f),
                                              Point2<float>(3.0f, 1.0f), Point2<float>(1.0f, 1.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                 Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionMinuendInsideContactEdge() { //see polygonsSubtractionMinuendInsideContactEdge.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-5.07582092f, 0.225821018f), Point2<float>(5.97582006f, 0.225821018f),
                                              Point2<float>(-5.07582092f, -10.8258209f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(5.0446701f, -10.9196606f), Point2<float>(-5.9446702f, -10.9196606f),
                                                 Point2<float>(-5.9446702f, 0.31965971f), Point2<float>(5.0446701f, 0.31965971f),
                                                 Point2<float>(5.0446701f, 0.225821018f), Point2<float>(5.97582102f, 0.225821018f),
                                                 Point2<float>(5.97582102f, -10.8258209f), Point2<float>(5.0446701f, -10.8258209f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionMiddlePoint() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0f, 2.0f), Point2<float>(5.0f, 2.0f), Point2<float>(6.0f, 1.0f),
                                              Point2<float>(5.0f, 0.0f), Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 1.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.0f, 3.0f), Point2<float>(4.5f, 1.5f), Point2<float>(4.0f, -1.0f),
                                                 Point2<float>(3.0f, 1.0f), Point2<float>(2.0f, -1.0f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(1.0f, 0.0f), Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 2.0f),
                                                                                 Point2<float>(2.0f, 2.0f), Point2<float>(2.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(4.2f, 0.0f), Point2<float>(4.5f, 1.5f), Point2<float>(3.666666f, 2.0f),
                                                                                 Point2<float>(5.0f, 2.0f), Point2<float>(6.0f, 1.0f), Point2<float>(5.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[2].getCwPoints(), {Point2<float>(2.5f, 0.0f), Point2<float>(3.0f, 1.0f), Point2<float>(3.5f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 3.0f),
                                                 Point2<float>(2.0f, 3.0f), Point2<float>(2.0f, 4.0f), Point2<float>(1.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f), Point2<float>(2.0f, 4.0f),
                                                                                 Point2<float>(2.0f, 3.0f), Point2<float>(3.0f, 3.0f), Point2<float>(3.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 3.0f),
                                                 Point2<float>(2.0f, 3.0f), Point2<float>(2.0f, 4.0f), Point2<float>(0.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f),
                                                                                 Point2<float>(0.0f, 4.0f), Point2<float>(2.0f, 4.0f), Point2<float>(2.0f, 3.0f),
                                                                                 Point2<float>(3.0f, 3.0f), Point2<float>(3.0f, 4.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge3() { //see polygonsSubtractionSameEdge3.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-0.175821006f, -4.67417908f), Point2<float>(1.07581997f, -4.67417908f),
                                              Point2<float>(-0.175821006f, -5.9258213f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.14467001f, -6.01966f), Point2<float>(-1.04466999f, -6.01966f),
                                                 Point2<float>(-1.04466999f, -4.58034039f), Point2<float>(0.14467001f, -4.58034039f),
                                                 Point2<float>(0.14467001f, -4.67417908f), Point2<float>(1.07582092f, -4.67417908f),
                                                 Point2<float>(1.07582092f, -5.9258213f), Point2<float>(0.14467001f, -5.9258213f)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge4() { //see polygonsSubtractionSameEdge4.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.76618004f, -6.51731586f), Point2<float>(-5.76618004f, -6.51731586f),
                                              Point2<float>(-5.76618004f, 1.51731598f), Point2<float>(5.76618004f, 1.51731598f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(-4.24466991f, -9.21965981f),Point2<float>(6.27905893f, 1.76014054f),
                                                 Point2<float>(5.76618004f, 0.700549841f), Point2<float>(5.76618004f, -4.86956978f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(-5.76618004f, -6.51731586f), Point2<float>(-5.76618004f, 1.51731598f),
                                                                                 Point2<float>(5.76618004f, 1.51731598f), Point2<float>(5.76618004f, 1.22503483f),
                                                                                 Point2<float>(-1.65457416f, -6.51731586f)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(1.97422659f, -6.51731586f), Point2<float>(5.76618004f, -4.86956978f),
                                                                                 Point2<float>(5.76618004f, -6.51731586f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(1.5f, 4.0f), Point2<float>(2.0f, 3.0f),
                                              Point2<float>(2.5f, 4.0f), Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 0.0f),
                                              Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 4.0f),
                                                 Point2<float>(1.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f), Point2<float>(1.5f, 4.0f),
                                                                                 Point2<float>(2.0f, 3.0f), Point2<float>(2.5f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(1.5f, 4.0f), Point2<float>(2.0f, 4.5f),
                                              Point2<float>(2.5f, 4.0f), Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 0.0f),
                                              Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 4.0f),
                                                 Point2<float>(1.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f),
                                                                                 Point2<float>(1.0f, 4.0f), Point2<float>(3.0f, 4.0f), Point2<float>(4.0f, 4.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 4.0f),
                                                 Point2<float>(2.5f, 4.0f), Point2<float>(2.0f, 3.0f), Point2<float>(1.5f, 4.0f),
                                                 Point2<float>(1.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f), Point2<float>(1.5f, 4.0f),
                                                                                 Point2<float>(2.0f, 3.0f), Point2<float>(2.5f, 4.0f), Point2<float>(4.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0f, 5.0f), Point2<float>(3.0f, 5.0f), Point2<float>(3.0f, 4.0f),
                                                 Point2<float>(2.5f, 4.0f), Point2<float>(2.0f, 4.5f), Point2<float>(1.5f, 4.0f),
                                                 Point2<float>(1.0f, 4.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 4.0f),
                                                                                 Point2<float>(4.0f, 4.0f), Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge5() { //see polygonsSubtractionTwoSameEdge5.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(4.76618004f, -5.51731586f), Point2<float>(-4.76618004f, -5.51731586f),
                                              Point2<float>(-4.76618004f, 0.517315984f), Point2<float>(4.76618004f, 0.517315984f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.76618004f, -3.86956978f), Point2<float>(-4.76618004f, -3.86956978f),
                                                 Point2<float>(-4.76618004f, 3.86957693f), Point2<float>(4.4309082f, 3.86957693f),
                                                 Point2<float>(4.76617956f, 3.74754858f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(-4.76618004f, -5.51731586f), Point2<float>(-4.76618004f, -3.86956978f),
                                                                                  Point2<float>(4.76618004f, -3.86956978f), Point2<float>(4.76618004f, -5.51731586f)});
}

void PolygonsSubtractionTest::polygonsSubtractionMaxIteration() { //see polygonsSubtractionMaxIteration.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.724967539f, -6.26921654f), Point2<float>(2.0384562f, -6.03173637f),
                                              Point2<float>(1.0561136f, -8.09460735f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.756835938f, -6.38183594f), Point2<float>(1.75683594f, -6.38183594f),
                                                 Point2<float>(1.75683594f, -6.43554688f), Point2<float>(0.756835938f, -6.61425781f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.724967539f, -6.26921654f), Point2<float>(2.0384562f, -6.03173637f),
                                                                                 Point2<float>(1.0561136f, -8.09460735f), Point2<float>(0.786597f, -6.608939f),
                                                                                 Point2<float>(1.75683594f, -6.43554688f), Point2<float>(1.75683594f, -6.38183594f),
                                                                                 Point2<float>(0.756835938f, -6.38183594f), Point2<float>(0.756836f, -6.444886f)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoIntersectionsOnEdge() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-0.00646264944f, -6.47982645f), Point2<float>(1.59619427f, -4.96815157f),
                                              Point2<float>(1.53988385f, -6.4921689f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.45019531f, -4.84667969f), Point2<float>(2.71777344f, -6.58496094f),
                                                 Point2<float>(0.471679688f, -6.90820312f), Point2<float>(0.204101562f, -5.16992188f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(1.59619427f, -4.96815157f), Point2<float>(1.596141f, -4.969589f),
                                                                                 Point2<float>(1.594405f, -4.969839f)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(-0.00646264944f, -6.47982645f), Point2<float>(0.353478f, -6.140319f),
                                                                                 Point2<float>(0.406246f, -6.483120f)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(3.0f, 4.0f), Point2<float>(3.0f, 5.0f), Point2<float>(5.0f, 5.0f),
                                                 Point2<float>(5.0f, 3.0f), Point2<float>(4.0f, 3.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 4.0f), Point2<float>(3.0f, 4.0f), Point2<float>(4.0f, 3.0f),
                                                                                 Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0f, 4.0f), Point2<float>(4.0f, 4.0f),
                                              Point2<float>(4.0f, 0.0f), Point2<float>(0.0f, 0.0f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(-1.0f, 2.0f), Point2<float>(2.0f, 15.0f),
                                                 Point2<float>(5.0f, 2.0f), Point2<float>(2.0f, 5.0f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 3.0f), Point2<float>(1.0f, 4.0f),
                                                                                 Point2<float>(3.0f, 4.0f), Point2<float>(4.0f, 3.0f), Point2<float>(4.0f, 0.0f)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner3() { //see polygonsSubtractionCorner3.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755f, -8.64567757f), Point2<float>(4.47754383f, -7.34843636f),
                                              Point2<float>(5.91046381f, -6.30504799f), Point2<float>(7.27159405f, -7.60229063f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0f, -6.0f), Point2<float>(7.44508f, -6.18886f),
                                                 Point2<float>(4.98730469f, -6.67675781f), Point2<float>(5.31445312f, -7.43554688f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.47754383f, -7.34843636f), Point2<float>(4.93181419f, -7.01765728f),
                                                                                 Point2<float>(5.31445312f, -7.43554688f), Point2<float>(5.08590603f, -6.90545416f),
                                                                                 Point2<float>(5.55465508f, -6.56413221f), Point2<float>(6.07411051f, -6.46101379f),
                                                                                 Point2<float>(7.27159405f, -7.60229063f), Point2<float>(5.8386755f, -8.64567757f)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner4() { //see polygonsSubtractionCorner4.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755f, -8.64567757f), Point2<float>(4.47754383f, -7.34843636f),
                                              Point2<float>(5.91046381f, -6.30504799f), Point2<float>(7.27159405f, -7.60229063f)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0f, -6.0f), Point2<float>(70.5371094f, 20.34375f),
                                                 Point2<float>(4.98730469f, -6.67675781f), Point2<float>(5.31445312f, -7.43554688f)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(5.8386755f, -8.64567757f), Point2<float>(4.47754383f, -7.34843636f),
                                                                                 Point2<float>(4.93181419f, -7.01765728f), Point2<float>(5.31445312f, -7.43554688f),
                                                                                 Point2<float>(5.08590603f, -6.90545416f), Point2<float>(5.91046381f, -6.30504799f),
                                                                                 Point2<float>(7.27159405f, -7.60229063f)});
}

CppUnit::Test* PolygonsSubtractionTest::suite() {
    auto* suite = new CppUnit::TestSuite("PolygonsSubtractionTest");

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInsideContactPoint", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactPoint));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInsideContactEdge", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactEdge));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendOutside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMinuendInside", &PolygonsSubtractionTest::polygonsSubtractionMinuendInside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMinuendInsideContactEdge", &PolygonsSubtractionTest::polygonsSubtractionMinuendInsideContactEdge));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMiddlePoint", &PolygonsSubtractionTest::polygonsSubtractionMiddlePoint));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge3", &PolygonsSubtractionTest::polygonsSubtractionSameEdge3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge4", &PolygonsSubtractionTest::polygonsSubtractionSameEdge4));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge3", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge4", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge5", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge5));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMaxIteration", &PolygonsSubtractionTest::polygonsSubtractionMaxIteration));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoIntersectionsOnEdge", &PolygonsSubtractionTest::polygonsSubtractionTwoIntersectionsOnEdge));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner1", &PolygonsSubtractionTest::polygonsSubtractionCorner1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner2", &PolygonsSubtractionTest::polygonsSubtractionCorner2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner3", &PolygonsSubtractionTest::polygonsSubtractionCorner3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner4", &PolygonsSubtractionTest::polygonsSubtractionCorner4));

    return suite;
}
