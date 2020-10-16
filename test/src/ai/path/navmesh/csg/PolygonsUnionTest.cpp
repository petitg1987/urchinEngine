#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsUnionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsUnionTest::onePolygonUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)});
}

void PolygonsUnionTest::twoPolygonsNoUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(2.0, 0.0), Point2<float>(2.5, 1.0), Point2<float>(3.0, 0.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2<float>(2.0, 0.0), Point2<float>(2.5, 1.0), Point2<float>(3.0, 0.0)});
}

void PolygonsUnionTest::polygonInsideAnother() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(1.5, 4.0), Point2<float>(3.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(1.0, 1.0), Point2<float>(1.5, 2.0), Point2<float>(2.0, 1.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(1.5, 4.0), Point2<float>(3.0, 0.0)});
}

void PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(1.0, 0.0), Point2<float>(2.0, -0.01), Point2<float>(2.0, -1.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0, 0.0), Point2<float>(3.0, 0.0), Point2<float>(0.0, -5.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, -5.0), Point2<float>(0.0, 0.0),
                                                                           Point2<float>(1.0, 0.0), Point2<float>(3.0, 0.0)});
}

void PolygonsUnionTest::twoPolygonsWithHoleInside() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.530539632, -4.50937748), Point2<float>(0.469382435, -4.50937748),
                                              Point2<float>(0.469382435, -5.50917721), Point2<float>(-0.530539632, -5.50917721)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0802265704, -7.24355221), Point2<float>(-0.0744862929, -7.2450161),
                                              Point2<float>(-0.0753405541, -7.18082476), Point2<float>(-1.51282442, -6.91251755),
                                              Point2<float>(-1.29546773, -5.63524199), Point2<float>(-1.14017522, -5.3795886),
                                              Point2<float>(-0.399924427, -5.51769829), Point2<float>(-0.399924427, -4.35019779),
                                              Point2<float>(0.899924397, -4.35019779), Point2<float>(0.899924397, -4.96294832),
                                              Point2<float>(2.03855228, -5.62088203), Point2<float>(1.66762424, -6.15682983),
                                              Point2<float>(1.68166137, -7.22853851), Point2<float>(0.921211898, -7.23561811),
                                              Point2<float>(0.677094221, -7.58849049)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 17);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(1.0, 1.0), Point2<float>(2.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0, 0.5), Point2<float>(1.0, 1.5), Point2<float>(2.0, 0.5)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.5, 0.5), Point2<float>(0.0, 0.5), Point2<float>(1.0, 1.5),
                                                                           Point2<float>(2.0, 0.5), Point2<float>(1.5, 0.5), Point2<float>(2.0, 0.0)});
}

void PolygonsUnionTest::twoPolygonsUnionXAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-1.52582097, 5.9758215), Point2<float>(-0.174179018, 5.9758215),
                                              Point2<float>(-0.174179018, 4.62417889), Point2<float>(-1.52582097, 4.62417889)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821018, 5.9758215), Point2<float>(1.12582099, 5.9758215),
                                              Point2<float>(1.12582099, 4.62417889), Point2<float>(-0.225821018, 4.62417889)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582099, 4.62417889), Point2<float>(-0.174179018, 4.62417889),
                                                                           Point2<float>(-1.52582097, 4.62417889), Point2<float>(-1.52582097, 5.9758215),
                                                                           Point2<float>(-0.225821018, 5.9758215), Point2<float>(1.12582099, 5.9758215)});
}

void PolygonsUnionTest::twoPolygonsUnionYAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.0, 2.0), Point2<float>(2.0, 2.0),
                                              Point2<float>(2.0, -2.0), Point2<float>(-2.0, -2.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-2.0, 3.0), Point2<float>(2.0, 3.0),
                                              Point2<float>(2.0, 1.0), Point2<float>(-2.0, 1.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(2.0, -2.0), Point2<float>(-2.0, -2.0), Point2<float>(-2.0, 1.0),
                                                                           Point2<float>(-2.0, 3.0), Point2<float>(2.0, 3.0), Point2<float>(2.0, 2.0)});
}

void PolygonsUnionTest::twoPolygonsUnionYAlmostAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.61617994, 1.71957302), Point2<float>(2.61617994, 1.71957302),
                                              Point2<float>(2.61617994, -1.71957302), Point2<float>(-2.61617994, -1.71957302)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-2.6161797, 3.36731601), Point2<float>(2.6161797, 3.36731601),
                                              Point2<float>(2.6161797, 1.63268399), Point2<float>(-2.6161797, 1.63268399)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(2.61617994, -1.71957302), Point2<float>(-2.61617994, -1.71957302),
                                                                           Point2<float>(-2.61617994, 1.63268399), Point2<float>(-2.6161797, 3.36731601),
                                                                           Point2<float>(2.6161797, 3.36731601), Point2<float>(2.61617994, 1.71957302)});
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge1() {
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(-419, -5371), Point2<long long>(912, -5200),
                                                  Point2<long long>(912, -5780), Point2<long long>(-419, -5780)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(-51, -5324), Point2<long long>(973, -5192),
                                                  Point2<long long>(973, -5939), Point2<long long>(-51, -5939)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonLongLongEquals(polygonUnion[0].getCwPoints(), {Point2<long long>(973, -5939), Point2<long long>(-51, -5939),
                                                                              Point2<long long>(-51, -5780), Point2<long long>(-419, -5780),
                                                                              Point2<long long>(-419, -5371), Point2<long long>(595, -5241),
                                                                              Point2<long long>(973, -5192)});
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2() {
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(49050, 20245), Point2<long long>(49156, 19704), Point2<long long>(48751, 20265)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(47875, 19997), Point2<long long>(49098, 19997), Point2<long long>(49420, 18354),
                                                  Point2<long long>(48381, 18423), Point2<long long>(47875, 19124)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 9);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3() {
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(922, -5526), Point2<long long>(922, -5786), Point2<long long>(-53, -5786)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(-51, -5785), Point2<long long>(973, -5512),
                                                  Point2<long long>(973, -5939), Point2<long long>(-51, -5939)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 6);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge4() { //see polygonsUnionAlmostSameEdge4.ggb
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(-198, -6084), Point2<long long>(-214, -4811),
                                                  Point2<long long>(148, -4806), Point2<long long>(148, -5728)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(-199, -6084), Point2<long long>(-214, -4811), Point2<long long>(-203, -4811),
                                                  Point2<long long>(511, -4801), Point2<long long>(1061, -4795), Point2<long long>(417, -5454),
                                                  Point2<long long>(244, -5632)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 10);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge5() { //see polygonsUnionAlmostSameEdge5.ggb
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(829, -2960), Point2<long long>(1029, -2872), Point2<long long>(882, -3304)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(829, -2961), Point2<long long>(2379, -2275),
                                                  Point2<long long>(1994, -3397), Point2<long long>(896, -3397)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 7);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge6() { //see polygonsUnionAlmostSameEdge6.ggb
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.56617975, -1.58268416), Point2<float>(2.56617975, -1.58268416),
                                              Point2<float>(2.56617975, -3.21731615), Point2<float>(-2.56617975, -3.21731615)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(55.3621445, -75.836937), Point2<float>(42.3322601, -75.836937),
                                              Point2<float>(-55.8955078, 62.2369347), Point2<float>(-42.8656235, 62.2369347),
                                              Point2<float>(0.222888947, 1.66956329), Point2<float>(2.56617999, 1.66957486),
                                              Point2<float>(2.56617999, -1.62428343)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(55.3621445, -75.836937), Point2<float>(42.3322601, -75.836937),
                                                                           Point2<float>(-55.8955078, 62.2369347), Point2<float>(-42.8656235, 62.2369347),
                                                                           Point2<float>(0.222888947, 1.66956329), Point2<float>(2.56617999, 1.66957486),
                                                                           Point2<float>(2.56617999, -1.58268416), Point2<float>(2.56617999, -1.62428343)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint1() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, -1.0), Point2<float>(0.0, 0.0), Point2<float>(2.0, 1.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0), Point2<float>(1.0, 0.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, -1.0), Point2<float>(0.0, 0.0), Point2<float>(0.0, 1.0),
                                                                           Point2<float>(0.666666, 0.333333), Point2<float>(2.0, 1.0)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint2() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-30.9001007, 17.9001007), Point2<float>(-30.5001011, 17.8999023),
                                              Point2<float>(-30.6999035, 17.5000992)};

    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-30.9001007, 17.9001007), Point2<float>(-30.7001019, 18.5000992),
                                              Point2<float>(-30.6999035, 17.9001007)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(polygonUnion[1].getCwPoints().size(), 3);
    //no points check
}

void PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(0.0, 2.0), Point2<float>(2.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0, 1.0), Point2<float>(0.5, 1.0), Point2<float>(0.5, 0.5)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 2.0), Point2<float>(2.0, 0.0)});
}

void PolygonsUnionTest::twoIdenticalTriangles() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0), Point2<float>(1.0, 0.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.5, 1.0),
                                                                           Point2<float>(1.0, 0.0)});
}

void PolygonsUnionTest::twoIdenticalSquares() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.225821, 5.97582), Point2<float>(1.12582, 5.97582),
                                             Point2<float>(1.12582, 4.62418), Point2<float>(-0.225821, 4.62418)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821, 5.97582), Point2<float>(1.12582, 5.97582),
                                             Point2<float>(1.12582, 4.62418), Point2<float>(-0.225821, 4.62418)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582, 4.62418), Point2<float>(-0.225821, 4.62418),
                                                                           Point2<float>(-0.225821, 5.97582), Point2<float>(1.12582, 5.97582)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares1() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.225821018, -4.62417889), Point2<float>(1.12581992, -4.62417889),
                                              Point2<float>(1.12581992, -5.9758215), Point2<float>(-0.225821018, -5.9758215)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821018, -4.62417889), Point2<float>(1.12582099, -4.62417889),
                                              Point2<float>(1.12582099, -5.9758215), Point2<float>(-0.225821018, -5.9758215)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582099, -5.9758215), Point2<float>(-0.225821018, -5.9758215),
                                                                           Point2<float>(-0.225821018, -4.62417889), Point2<float>(1.12582099, -4.62417889)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares2() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(1.0258255, -5.87582397), Point2<float>(-0.125823975, -5.87582397),
                                              Point2<float>(-0.125823975, -4.72417879), Point2<float>(1.02582097, -4.72417879)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(1.02582097, -5.87582159), Point2<float>(-0.125820994, -5.87582159),
                                              Point2<float>(-0.125821024, -4.72417879), Point2<float>(1.02582097, -4.72417879)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.0258255, -5.87582397), Point2<float>(-0.125823975, -5.87582397),
                                                                           Point2<float>(-0.125823975, -4.72417879), Point2<float>(1.02582097, -4.72417879)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares3() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.175821006, -4.67417908), Point2<float>(1.07582092, -4.67417908),
                                              Point2<float>(1.07582092, -5.9258213), Point2<float>(-0.175821006, -5.9258213)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.175821006, -4.67417908), Point2<float>(1.07581997, -4.67417908),
                                              Point2<float>(1.07581997, -5.9258213), Point2<float>(-0.175821006, -5.9258213)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.07582, -5.9258213), Point2<float>(-0.175821006, -5.9258213),
                                                                           Point2<float>(-0.175821006, -4.67417908), Point2<float>(1.07582092, -4.67417908)});
}

void PolygonsUnionTest::twoAlmostIdenticalTrapeze() { //see polygonsUnionAlmostIdenticalTrapeze.ggb
    std::vector<Point2<long long>> polyPoints1 = {Point2<long long>(748, -4979), Point2<long long>(922, -5010),
                                                  Point2<long long>(922, -5786), Point2<long long>(541, -5786)};
    std::vector<Point2<long long>> polyPoints2 = {Point2<long long>(747, -4979), Point2<long long>(973, -5019),
                                                  Point2<long long>(973, -5939), Point2<long long>(502, -5939)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon<long long>("p1", std::move(polyPoints1)), CSGPolygon<long long>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonLongLongEquals(polygonUnion[0].getCwPoints(), {Point2<long long>(973, -5939), Point2<long long>(502, -5939), Point2<long long>(747, -4979),
                                                                              Point2<long long>(748, -4979), Point2<long long>(922, -5010), Point2<long long>(973, -5019)});
}

void PolygonsUnionTest::threePolygonsUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0, 0.0), Point2<float>(0.0, 2.0),
                                              Point2<float>(2.0, 2.0), Point2<float>(2.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.5, 1.0), Point2<float>(1.0, 3.0), Point2<float>(1.5, 1.0)};
    std::vector<Point2<float>> polyPoints3 = {Point2<float>(1.0, 0.5), Point2<float>(1.0, 1.5), Point2<float>(3.0, 1.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2)), CSGPolygon<float>("p3", std::move(polyPoints3))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0, 0.0), Point2<float>(0.0, 2.0), Point2<float>(0.75, 2.0),
                                                                           Point2<float>(1.0, 3.0), Point2<float>(1.25, 2.0), Point2<float>(2.0, 2.0),
                                                                           Point2<float>(2.0, 1.25), Point2<float>(3.0, 1.0), Point2<float>(2.0, 0.75),
                                                                           Point2<float>(2.0, 0.0)});
}

void PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(4.0, 0.0), Point2<float>(4.0, 2.0),
                                              Point2<float>(6.0, 2.0), Point2<float>(6.0, 0.0)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.5, 1.0), Point2<float>(1.0, 3.0), Point2<float>(1.5, 1.0)};
    std::vector<Point2<float>> polyPoints3 = {Point2<float>(1.0, 0.5), Point2<float>(1.0, 1.5), Point2<float>(3.0, 1.0)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2)), CSGPolygon<float>("p3", std::move(polyPoints3))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(4.0, 0.0), Point2<float>(4.0, 2.0), Point2<float>(6.0, 2.0),
                                                                           Point2<float>(6.0, 0.0)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2<float>(1.0, 0.5), Point2<float>(1.0, 1.0), Point2<float>(0.5, 1.0),
                                                                           Point2<float>(1.0, 3.0), Point2<float>(1.4, 1.4), Point2<float>(3.0, 1.0)});
}

CppUnit::Test *PolygonsUnionTest::suite() {
    auto *suite = new CppUnit::TestSuite("PolygonsUnionTest");

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("onePolygonUnion", &PolygonsUnionTest::onePolygonUnion));

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNoUnion", &PolygonsUnionTest::twoPolygonsNoUnion));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnother", &PolygonsUnionTest::polygonInsideAnother));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("polygonInsideAnotherTouchInOnePoint", &PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint));

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsWithHoleInside", &PolygonsUnionTest::twoPolygonsWithHoleInside));

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnion", &PolygonsUnionTest::twoPolygonsUnion));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionXAligned", &PolygonsUnionTest::twoPolygonsUnionXAligned));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionYAligned", &PolygonsUnionTest::twoPolygonsUnionYAligned));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionYAlmostAligned", &PolygonsUnionTest::twoPolygonsUnionYAlmostAligned));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge1", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge2", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge3", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge4", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge4));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge5", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge5));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAlmostSameEdge6", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge6));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsTouchInOnePoint1", &PolygonsUnionTest::twoPolygonsTouchInOnePoint1));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsTouchInOnePoint2", &PolygonsUnionTest::twoPolygonsTouchInOnePoint2));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsNestedTouchInOnePoint", &PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint));

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalTriangles", &PolygonsUnionTest::twoIdenticalTriangles));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoIdenticalSquares", &PolygonsUnionTest::twoIdenticalSquares));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalSquares1", &PolygonsUnionTest::twoAlmostIdenticalSquares1));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalSquares2", &PolygonsUnionTest::twoAlmostIdenticalSquares2));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalSquares3", &PolygonsUnionTest::twoAlmostIdenticalSquares3));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoAlmostIdenticalTrapeze", &PolygonsUnionTest::twoAlmostIdenticalTrapeze));

    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));
    suite->addTest(new CppUnit::TestCaller<PolygonsUnionTest>("twoPolygonsUnionAndSeparatePolygon", &PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon));

    return suite;
}
