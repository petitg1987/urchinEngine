#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsSubtractionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactPoint() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(4.0, 3.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactEdge() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(4.0, 3.0),
                                                 Point2<float>(4.0, 1.0), Point2<float>(1.0, 1.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0),
                                                                                 Point2<float>(4.0, 4.0), Point2<float>(4.0, 3.0),
                                                                                 Point2<float>(1.0, 3.0), Point2<float>(1.0, 1.0),
                                                                                 Point2<float>(4.0, 1.0), Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 7.0), Point2<float>(3.0, 7.0),
                                                 Point2<float>(3.0, 5.0), Point2<float>(1.0, 5.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionMinuendInside() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 3.0), Point2<float>(3.0, 3.0),
                                              Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionMinuendInsideContactEdge() { //see polygonsSubtractionMinuendInsideContactEdge.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-5.07582092, 0.225821018), Point2<float>(5.97582006, 0.225821018),
                                              Point2<float>(-5.07582092, -10.8258209)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(5.0446701, -10.9196606), Point2<float>(-5.9446702, -10.9196606),
                                                 Point2<float>(-5.9446702, 0.31965971), Point2<float>(5.0446701, 0.31965971),
                                                 Point2<float>(5.0446701, 0.225821018), Point2<float>(5.97582102, 0.225821018),
                                                 Point2<float>(5.97582102, -10.8258209), Point2<float>(5.0446701, -10.8258209)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionMiddlePoint() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 2.0), Point2<float>(5.0, 2.0), Point2<float>(6.0, 1.0),
                                              Point2<float>(5.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.0, 3.0), Point2<float>(4.5, 1.5), Point2<float>(4.0, -1.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(2.0, -1.0)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 3);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(1.0, 0.0), Point2<float>(0.0, 1.0), Point2<float>(1.0, 2.0),
                                                                                 Point2<float>(2.0, 2.0), Point2<float>(2.0, 0.0)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(4.2, 0.0), Point2<float>(4.5, 1.5), Point2<float>(3.666666, 2.0),
                                                                                 Point2<float>(5.0, 2.0), Point2<float>(6.0, 1.0), Point2<float>(5.0, 0.0)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[2].getCwPoints(), {Point2<float>(2.5, 0.0), Point2<float>(3.0, 1.0), Point2<float>(3.5, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(2.0, 3.0), Point2<float>(2.0, 4.0), Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0), Point2<float>(2.0, 4.0),
                                                                                 Point2<float>(2.0, 3.0), Point2<float>(3.0, 3.0), Point2<float>(3.0, 4.0),
                                                                                 Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(2.0, 3.0), Point2<float>(2.0, 4.0), Point2<float>(0.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0),
                                                                                 Point2<float>(0.0, 4.0), Point2<float>(2.0, 4.0), Point2<float>(2.0, 3.0),
                                                                                 Point2<float>(3.0, 3.0), Point2<float>(3.0, 4.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge3() { //see polygonsSubtractionSameEdge3.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-0.175821006, -4.67417908), Point2<float>(1.07581997, -4.67417908),
                                              Point2<float>(-0.175821006, -5.9258213)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.14467001, -6.01966), Point2<float>(-1.04466999, -6.01966),
                                                 Point2<float>(-1.04466999, -4.58034039), Point2<float>(0.14467001, -4.58034039),
                                                 Point2<float>(0.14467001, -4.67417908), Point2<float>(1.07582092, -4.67417908),
                                                 Point2<float>(1.07582092, -5.9258213), Point2<float>(0.14467001, -5.9258213)};

    bool subtrahendInsideMinuend;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)), subtrahendInsideMinuend);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge4() { //see polygonsSubtractionSameEdge4.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.76618004, -6.51731586), Point2<float>(-5.76618004, -6.51731586),
                                              Point2<float>(-5.76618004, 1.51731598), Point2<float>(5.76618004, 1.51731598)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(-4.24466991, -9.21965981),Point2<float>(6.27905893, 1.76014054),
                                                 Point2<float>(5.76618004, 0.700549841), Point2<float>(5.76618004, -4.86956978)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(-5.76618004, -6.51731586), Point2<float>(-5.76618004, 1.51731598),
                                                                                 Point2<float>(5.76618004, 1.51731598), Point2<float>(5.76618004, 1.22503483),
                                                                                 Point2<float>(-1.65457416, -6.51731586)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(1.97422659, -6.51731586), Point2<float>(5.76618004, -4.86956978),
                                                                                 Point2<float>(5.76618004, -6.51731586)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0), Point2<float>(2.0, 3.0),
                                              Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0),
                                              Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0),
                                                                                 Point2<float>(2.0, 3.0), Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0),
                                                                                 Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0), Point2<float>(2.0, 4.5),
                                              Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0),
                                              Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0),
                                                                                 Point2<float>(1.0, 4.0), Point2<float>(3.0, 4.0), Point2<float>(4.0, 4.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(2.5, 4.0), Point2<float>(2.0, 3.0), Point2<float>(1.5, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0),
                                                                                 Point2<float>(2.0, 3.0), Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0),
                                                                                 Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(2.5, 4.0), Point2<float>(2.0, 4.5), Point2<float>(1.5, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 4.0),
                                                                                 Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge5() { //see polygonsSubtractionTwoSameEdge5.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(4.76618004, -5.51731586), Point2<float>(-4.76618004, -5.51731586),
                                              Point2<float>(-4.76618004, 0.517315984), Point2<float>(4.76618004, 0.517315984)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.76618004, -3.86956978), Point2<float>(-4.76618004, -3.86956978),
                                                 Point2<float>(-4.76618004, 3.86957693), Point2<float>(4.4309082, 3.86957693),
                                                 Point2<float>(4.76617956, 3.74754858)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(-4.76618004, -5.51731586), Point2<float>(-4.76618004, -3.86956978),
                                                                                  Point2<float>(4.76618004, -3.86956978), Point2<float>(4.76618004, -5.51731586)});
}

void PolygonsSubtractionTest::polygonsSubtractionMaxIteration() { //see polygonsSubtractionMaxIteration.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.724967539, -6.26921654), Point2<float>(2.0384562, -6.03173637),
                                              Point2<float>(1.0561136, -8.09460735)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.756835938, -6.38183594), Point2<float>(1.75683594, -6.38183594),
                                                 Point2<float>(1.75683594, -6.43554688), Point2<float>(0.756835938, -6.61425781)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.724967539, -6.26921654), Point2<float>(2.0384562, -6.03173637),
                                                                                 Point2<float>(1.0561136, -8.09460735), Point2<float>(0.786597, -6.608939),
                                                                                 Point2<float>(1.75683594, -6.43554688), Point2<float>(1.75683594, -6.38183594),
                                                                                 Point2<float>(0.756835938, -6.38183594), Point2<float>(0.756836, -6.444886)});
}

void PolygonsSubtractionTest::polygonsSubtractionTwoIntersectionsOnEdge() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-0.00646264944, -6.47982645), Point2<float>(1.59619427, -4.96815157),
                                              Point2<float>(1.53988385, -6.4921689)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.45019531, -4.84667969), Point2<float>(2.71777344, -6.58496094),
                                                 Point2<float>(0.471679688, -6.90820312), Point2<float>(0.204101562, -5.16992188)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(1.59619427, -4.96815157), Point2<float>(1.596141, -4.969589),
                                                                                 Point2<float>(1.594405, -4.969839)});
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[1].getCwPoints(), {Point2<float>(-0.00646264944, -6.47982645), Point2<float>(0.353478, -6.140319),
                                                                                 Point2<float>(0.406246, -6.483120)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner1() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(3.0, 4.0), Point2<float>(3.0, 5.0), Point2<float>(5.0, 5.0),
                                                 Point2<float>(5.0, 3.0), Point2<float>(4.0, 3.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 4.0), Point2<float>(3.0, 4.0), Point2<float>(4.0, 3.0),
                                                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner2() {
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(-1.0, 2.0), Point2<float>(2.0, 15.0),
                                                 Point2<float>(5.0, 2.0), Point2<float>(2.0, 5.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 3.0), Point2<float>(1.0, 4.0),
                                                                                 Point2<float>(3.0, 4.0), Point2<float>(4.0, 3.0), Point2<float>(4.0, 0.0)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner3() { //see polygonsSubtractionCorner3.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755, -8.64567757), Point2<float>(4.47754383, -7.34843636),
                                              Point2<float>(5.91046381, -6.30504799), Point2<float>(7.27159405, -7.60229063)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0, -6.0), Point2<float>(7.44508, -6.18886),
                                                 Point2<float>(4.98730469, -6.67675781), Point2<float>(5.31445312, -7.43554688)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(4.47754383, -7.34843636), Point2<float>(4.93181419, -7.01765728),
                                                                                 Point2<float>(5.31445312, -7.43554688), Point2<float>(5.08590603, -6.90545416),
                                                                                 Point2<float>(5.55465508, -6.56413221), Point2<float>(6.07411051, -6.46101379),
                                                                                 Point2<float>(7.27159405, -7.60229063), Point2<float>(5.8386755, -8.64567757)});
}

void PolygonsSubtractionTest::polygonsSubtractionCorner4() { //see polygonsSubtractionCorner4.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755, -8.64567757), Point2<float>(4.47754383, -7.34843636),
                                              Point2<float>(5.91046381, -6.30504799), Point2<float>(7.27159405, -7.60229063)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0, -6.0), Point2<float>(70.5371094, 20.34375),
                                                 Point2<float>(4.98730469, -6.67675781), Point2<float>(5.31445312, -7.43554688)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", std::move(minuendPoly)), CSGPolygon<float>("subtrahend", std::move(subtrahendPoly)));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonSubtraction[0].getCwPoints(), {Point2<float>(5.8386755, -8.64567757), Point2<float>(4.47754383, -7.34843636),
                                                                                 Point2<float>(4.93181419, -7.01765728), Point2<float>(5.31445312, -7.43554688),
                                                                                 Point2<float>(5.08590603, -6.90545416), Point2<float>(5.91046381, -6.30504799),
                                                                                 Point2<float>(7.27159405, -7.60229063)});
}

CppUnit::Test *PolygonsSubtractionTest::suite() {
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
