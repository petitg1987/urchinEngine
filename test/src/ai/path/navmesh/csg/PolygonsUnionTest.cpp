#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/navmesh/csg/PolygonsUnionTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PolygonsUnionTest::onePolygonUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsNoUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(2.0f, 0.0f), Point2<float>(2.5f, 1.0f), Point2<float>(3.0f, 0.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2<float>(2.0f, 0.0f), Point2<float>(2.5f, 1.0f), Point2<float>(3.0f, 0.0f)});
}

void PolygonsUnionTest::polygonInsideAnother() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(1.5f, 4.0f), Point2<float>(3.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(1.0f, 1.0f), Point2<float>(1.5f, 2.0f), Point2<float>(2.0f, 1.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(1.5f, 4.0f), Point2<float>(3.0f, 0.0f)});
}

void PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(1.0f, 0.0f), Point2<float>(2.0f, -0.01f), Point2<float>(2.0f, -1.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0f, 0.0f), Point2<float>(3.0f, 0.0f), Point2<float>(0.0f, -5.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, -5.0f), Point2<float>(0.0f, 0.0f),
                                                                           Point2<float>(1.0f, 0.0f), Point2<float>(3.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsWithHoleInside() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.530539632f, -4.50937748f), Point2<float>(0.469382435f, -4.50937748f),
                                              Point2<float>(0.469382435f, -5.50917721f), Point2<float>(-0.530539632f, -5.50917721f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0802265704f, -7.24355221f), Point2<float>(-0.0744862929f, -7.2450161f),
                                              Point2<float>(-0.0753405541f, -7.18082476f), Point2<float>(-1.51282442f, -6.91251755f),
                                              Point2<float>(-1.29546773f, -5.63524199f), Point2<float>(-1.14017522f, -5.3795886f),
                                              Point2<float>(-0.399924427f, -5.51769829f), Point2<float>(-0.399924427f, -4.35019779f),
                                              Point2<float>(0.899924397f, -4.35019779f), Point2<float>(0.899924397f, -4.96294832f),
                                              Point2<float>(2.03855228f, -5.62088203f), Point2<float>(1.66762424f, -6.15682983f),
                                              Point2<float>(1.68166137f, -7.22853851f), Point2<float>(0.921211898f, -7.23561811f),
                                              Point2<float>(0.677094221f, -7.58849049f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 17);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnion() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(1.0f, 1.0f), Point2<float>(2.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0f, 0.5f), Point2<float>(1.0f, 1.5f), Point2<float>(2.0f, 0.5f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 0.5f), Point2<float>(0.0f, 0.5f), Point2<float>(1.0f, 1.5f),
                                                                           Point2<float>(2.0f, 0.5f), Point2<float>(1.5f, 0.5f), Point2<float>(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionXAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-1.52582097f, 5.9758215f), Point2<float>(-0.174179018f, 5.9758215f),
                                              Point2<float>(-0.174179018f, 4.62417889f), Point2<float>(-1.52582097f, 4.62417889f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821018f, 5.9758215f), Point2<float>(1.12582099f, 5.9758215f),
                                              Point2<float>(1.12582099f, 4.62417889f), Point2<float>(-0.225821018f, 4.62417889f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582099f, 4.62417889f), Point2<float>(-0.174179018f, 4.62417889f),
                                                                           Point2<float>(-1.52582097f, 4.62417889f), Point2<float>(-1.52582097f, 5.9758215f),
                                                                           Point2<float>(-0.225821018f, 5.9758215f), Point2<float>(1.12582099f, 5.9758215f)});
}

void PolygonsUnionTest::twoPolygonsUnionYAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.0f, 2.0f), Point2<float>(2.0f, 2.0f),
                                              Point2<float>(2.0f, -2.0f), Point2<float>(-2.0f, -2.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-2.0f, 3.0f), Point2<float>(2.0f, 3.0f),
                                              Point2<float>(2.0f, 1.0f), Point2<float>(-2.0f, 1.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(2.0f, -2.0f), Point2<float>(-2.0f, -2.0f), Point2<float>(-2.0f, 1.0f),
                                                                           Point2<float>(-2.0f, 3.0f), Point2<float>(2.0f, 3.0f), Point2<float>(2.0f, 2.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionYAlmostAligned() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.61617994f, 1.71957302f), Point2<float>(2.61617994f, 1.71957302f),
                                              Point2<float>(2.61617994f, -1.71957302f), Point2<float>(-2.61617994f, -1.71957302f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-2.6161797f, 3.36731601f), Point2<float>(2.6161797f, 3.36731601f),
                                              Point2<float>(2.6161797f, 1.63268399f), Point2<float>(-2.6161797f, 1.63268399f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(2.61617994f, -1.71957302f), Point2<float>(-2.61617994f, -1.71957302f),
                                                                           Point2<float>(-2.61617994f, 1.63268399f), Point2<float>(-2.6161797f, 3.36731601f),
                                                                           Point2<float>(2.6161797f, 3.36731601f), Point2<float>(2.61617994f, 1.71957302f)});
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
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-2.56617975f, -1.58268416f), Point2<float>(2.56617975f, -1.58268416f),
                                              Point2<float>(2.56617975f, -3.21731615f), Point2<float>(-2.56617975f, -3.21731615f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(55.3621445f, -75.836937f), Point2<float>(42.3322601f, -75.836937f),
                                              Point2<float>(-55.8955078f, 62.2369347f), Point2<float>(-42.8656235f, 62.2369347f),
                                              Point2<float>(0.222888947f, 1.66956329f), Point2<float>(2.56617999f, 1.66957486f),
                                              Point2<float>(2.56617999f, -1.62428343f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(55.3621445f, -75.836937f), Point2<float>(42.3322601f, -75.836937f),
                                                                           Point2<float>(-55.8955078f, 62.2369347f), Point2<float>(-42.8656235f, 62.2369347f),
                                                                           Point2<float>(0.222888947f, 1.66956329f), Point2<float>(2.56617999f, 1.66957486f),
                                                                           Point2<float>(2.56617999f, -1.58268416f), Point2<float>(2.56617999f, -1.62428343f)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint1() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, -1.0f), Point2<float>(0.0f, 0.0f), Point2<float>(2.0f, 1.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 1.0f), Point2<float>(1.0f, 0.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, -1.0f), Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 1.0f),
                                                                           Point2<float>(0.666666f, 0.333333f), Point2<float>(2.0f, 1.0f)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint2() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-30.9001007f, 17.9001007f), Point2<float>(-30.5001011f, 17.8999023f),
                                              Point2<float>(-30.6999035f, 17.5000992f)};

    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-30.9001007f, 17.9001007f), Point2<float>(-30.7001019f, 18.5000992f),
                                              Point2<float>(-30.6999035f, 17.9001007f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertUnsignedInt(polygonUnion[0].getCwPoints().size(), 3);
    AssertHelper::assertUnsignedInt(polygonUnion[1].getCwPoints().size(), 3);
    //no points check
}

void PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 2.0f), Point2<float>(2.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0f, 1.0f), Point2<float>(0.5f, 1.0f), Point2<float>(0.5f, 0.5f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 2.0f), Point2<float>(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoIdenticalTriangles() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 0.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.5f, 1.0f),
                                                                           Point2<float>(1.0f, 0.0f)});
}

void PolygonsUnionTest::twoIdenticalSquares() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.225821f, 5.97582f), Point2<float>(1.12582f, 5.97582f),
                                             Point2<float>(1.12582f, 4.62418f), Point2<float>(-0.225821f, 4.62418f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821f, 5.97582f), Point2<float>(1.12582f, 5.97582f),
                                             Point2<float>(1.12582f, 4.62418f), Point2<float>(-0.225821f, 4.62418f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582f, 4.62418f), Point2<float>(-0.225821f, 4.62418f),
                                                                           Point2<float>(-0.225821f, 5.97582f), Point2<float>(1.12582f, 5.97582f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares1() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.225821018f, -4.62417889f), Point2<float>(1.12581992f, -4.62417889f),
                                              Point2<float>(1.12581992f, -5.9758215f), Point2<float>(-0.225821018f, -5.9758215f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.225821018f, -4.62417889f), Point2<float>(1.12582099f, -4.62417889f),
                                              Point2<float>(1.12582099f, -5.9758215f), Point2<float>(-0.225821018f, -5.9758215f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.12582099f, -5.9758215f), Point2<float>(-0.225821018f, -5.9758215f),
                                                                           Point2<float>(-0.225821018f, -4.62417889f), Point2<float>(1.12582099f, -4.62417889f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares2() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(1.0258255f, -5.87582397f), Point2<float>(-0.125823975f, -5.87582397f),
                                              Point2<float>(-0.125823975f, -4.72417879f), Point2<float>(1.02582097f, -4.72417879f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(1.02582097f, -5.87582159f), Point2<float>(-0.125820994f, -5.87582159f),
                                              Point2<float>(-0.125821024f, -4.72417879f), Point2<float>(1.02582097f, -4.72417879f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.0258255f, -5.87582397f), Point2<float>(-0.125823975f, -5.87582397f),
                                                                           Point2<float>(-0.125823975f, -4.72417879f), Point2<float>(1.02582097f, -4.72417879f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares3() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(-0.175821006f, -4.67417908f), Point2<float>(1.07582092f, -4.67417908f),
                                              Point2<float>(1.07582092f, -5.9258213f), Point2<float>(-0.175821006f, -5.9258213f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(-0.175821006f, -4.67417908f), Point2<float>(1.07581997f, -4.67417908f),
                                              Point2<float>(1.07581997f, -5.9258213f), Point2<float>(-0.175821006f, -5.9258213f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(1.07582f, -5.9258213f), Point2<float>(-0.175821006f, -5.9258213f),
                                                                           Point2<float>(-0.175821006f, -4.67417908f), Point2<float>(1.07582092f, -4.67417908f)});
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
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 2.0f),
                                              Point2<float>(2.0f, 2.0f), Point2<float>(2.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 3.0f), Point2<float>(1.5f, 1.0f)};
    std::vector<Point2<float>> polyPoints3 = {Point2<float>(1.0f, 0.5f), Point2<float>(1.0f, 1.5f), Point2<float>(3.0f, 1.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2)), CSGPolygon<float>("p3", std::move(polyPoints3))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(0.0f, 0.0f), Point2<float>(0.0f, 2.0f), Point2<float>(0.75f, 2.0f),
                                                                           Point2<float>(1.0f, 3.0f), Point2<float>(1.25f, 2.0f), Point2<float>(2.0f, 2.0f),
                                                                           Point2<float>(2.0f, 1.25f), Point2<float>(3.0f, 1.0f), Point2<float>(2.0f, 0.75f),
                                                                           Point2<float>(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon() {
    std::vector<Point2<float>> polyPoints1 = {Point2<float>(4.0f, 0.0f), Point2<float>(4.0f, 2.0f),
                                              Point2<float>(6.0f, 2.0f), Point2<float>(6.0f, 0.0f)};
    std::vector<Point2<float>> polyPoints2 = {Point2<float>(0.5f, 1.0f), Point2<float>(1.0f, 3.0f), Point2<float>(1.5f, 1.0f)};
    std::vector<Point2<float>> polyPoints3 = {Point2<float>(1.0f, 0.5f), Point2<float>(1.0f, 1.5f), Point2<float>(3.0f, 1.0f)};

    std::vector<CSGPolygon<float>> allPolygons = {CSGPolygon<float>("p1", std::move(polyPoints1)), CSGPolygon<float>("p2", std::move(polyPoints2)), CSGPolygon<float>("p3", std::move(polyPoints3))};
    std::vector<CSGPolygon<float>> polygonUnion = PolygonsUnion<float>::instance()->unionPolygons(allPolygons);

    AssertHelper::assertUnsignedInt(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2<float>(4.0f, 0.0f), Point2<float>(4.0f, 2.0f), Point2<float>(6.0f, 2.0f),
                                                                           Point2<float>(6.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2<float>(1.0f, 0.5f), Point2<float>(1.0f, 1.0f), Point2<float>(0.5f, 1.0f),
                                                                           Point2<float>(1.0f, 3.0f), Point2<float>(1.4f, 1.4f), Point2<float>(3.0f, 1.0f)});
}

CppUnit::Test* PolygonsUnionTest::suite() {
    auto* suite = new CppUnit::TestSuite("PolygonsUnionTest");

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
