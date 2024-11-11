#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinAIEngine.h>

#include <ai/path/navmesh/csg/PolygonsUnionTest.h>
#include <AssertHelper.h>
using namespace urchin;

void PolygonsUnionTest::onePolygonUnion() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsNoUnion() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(2.0f, 0.0f), Point2(2.5f, 1.0f), Point2(3.0f, 0.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2(2.0f, 0.0f), Point2(2.5f, 1.0f), Point2(3.0f, 0.0f)});
}

void PolygonsUnionTest::polygonInsideAnother() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(1.5f, 4.0f), Point2(3.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(1.0f, 1.0f), Point2(1.5f, 2.0f), Point2(2.0f, 1.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(1.5f, 4.0f), Point2(3.0f, 0.0f)});
}

void PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint() {
    std::vector polyPoints1 = {Point2(1.0f, 0.0f), Point2(2.0f, -0.01f), Point2(2.0f, -1.0f)};
    std::vector polyPoints2 = {Point2(0.0f, 0.0f), Point2(3.0f, 0.0f), Point2(0.0f, -5.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, -5.0f), Point2(0.0f, 0.0f),
                                                                           Point2(1.0f, 0.0f), Point2(3.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsWithHoleInside() {
    std::vector polyPoints1 = {Point2(-0.530539632f, -4.50937748f), Point2(0.469382435f, -4.50937748f),
                                              Point2(0.469382435f, -5.50917721f), Point2(-0.530539632f, -5.50917721f)};
    std::vector polyPoints2 = {Point2(0.0802265704f, -7.24355221f), Point2(-0.0744862929f, -7.2450161f),
                                              Point2(-0.0753405541f, -7.18082476f), Point2(-1.51282442f, -6.91251755f),
                                              Point2(-1.29546773f, -5.63524199f), Point2(-1.14017522f, -5.3795886f),
                                              Point2(-0.399924427f, -5.51769829f), Point2(-0.399924427f, -4.35019779f),
                                              Point2(0.899924397f, -4.35019779f), Point2(0.899924397f, -4.96294832f),
                                              Point2(2.03855228f, -5.62088203f), Point2(1.66762424f, -6.15682983f),
                                              Point2(1.68166137f, -7.22853851f), Point2(0.921211898f, -7.23561811f),
                                              Point2(0.677094221f, -7.58849049f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 17);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnion() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(1.0f, 1.0f), Point2(2.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(0.0f, 0.5f), Point2(1.0f, 1.5f), Point2(2.0f, 0.5f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.5f, 0.5f), Point2(0.0f, 0.5f), Point2(1.0f, 1.5f),
                                                                           Point2(2.0f, 0.5f), Point2(1.5f, 0.5f), Point2(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionXAligned() {
    std::vector polyPoints1 = {Point2(-1.52582097f, 5.9758215f), Point2(-0.174179018f, 5.9758215f),
                                              Point2(-0.174179018f, 4.62417889f), Point2(-1.52582097f, 4.62417889f)};
    std::vector polyPoints2 = {Point2(-0.225821018f, 5.9758215f), Point2(1.12582099f, 5.9758215f),
                                              Point2(1.12582099f, 4.62417889f), Point2(-0.225821018f, 4.62417889f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(1.12582099f, 4.62417889f), Point2(-0.174179018f, 4.62417889f),
                                                                           Point2(-1.52582097f, 4.62417889f), Point2(-1.52582097f, 5.9758215f),
                                                                           Point2(-0.225821018f, 5.9758215f), Point2(1.12582099f, 5.9758215f)});
}

void PolygonsUnionTest::twoPolygonsUnionYAligned() {
    std::vector polyPoints1 = {Point2(-2.0f, 2.0f), Point2(2.0f, 2.0f),
                                              Point2(2.0f, -2.0f), Point2(-2.0f, -2.0f)};
    std::vector polyPoints2 = {Point2(-2.0f, 3.0f), Point2(2.0f, 3.0f),
                                              Point2(2.0f, 1.0f), Point2(-2.0f, 1.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(2.0f, -2.0f), Point2(-2.0f, -2.0f), Point2(-2.0f, 1.0f),
                                                                           Point2(-2.0f, 3.0f), Point2(2.0f, 3.0f), Point2(2.0f, 2.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionYAlmostAligned() {
    std::vector polyPoints1 = {Point2(-2.61617994f, 1.71957302f), Point2(2.61617994f, 1.71957302f),
                                              Point2(2.61617994f, -1.71957302f), Point2(-2.61617994f, -1.71957302f)};
    std::vector polyPoints2 = {Point2(-2.6161797f, 3.36731601f), Point2(2.6161797f, 3.36731601f),
                                              Point2(2.6161797f, 1.63268399f), Point2(-2.6161797f, 1.63268399f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(2.61617994f, -1.71957302f), Point2(-2.61617994f, -1.71957302f),
                                                                           Point2(-2.61617994f, 1.63268399f), Point2(-2.6161797f, 3.36731601f),
                                                                           Point2(2.6161797f, 3.36731601f), Point2(2.61617994f, 1.71957302f)});
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge1() {
    std::vector polyPoints1 = {Point2<long long>(-419, -5371), Point2<long long>(912, -5200),
                                                  Point2<long long>(912, -5780), Point2<long long>(-419, -5780)};
    std::vector polyPoints2 = {Point2<long long>(-51, -5324), Point2<long long>(973, -5192),
                                                  Point2<long long>(973, -5939), Point2<long long>(-51, -5939)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonLongLongEquals(polygonUnion[0].getCwPoints(), {Point2<long long>(973, -5939), Point2<long long>(-51, -5939),
                                                                              Point2<long long>(-51, -5780), Point2<long long>(-419, -5780),
                                                                              Point2<long long>(-419, -5371), Point2<long long>(595, -5241),
                                                                              Point2<long long>(973, -5192)});
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2() {
    std::vector polyPoints1 = {Point2<long long>(49050, 20245), Point2<long long>(49156, 19704), Point2<long long>(48751, 20265)};
    std::vector polyPoints2 = {Point2<long long>(47875, 19997), Point2<long long>(49098, 19997), Point2<long long>(49420, 18354),
                                                  Point2<long long>(48381, 18423), Point2<long long>(47875, 19124)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 9);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3() {
    std::vector polyPoints1 = {Point2<long long>(922, -5526), Point2<long long>(922, -5786), Point2<long long>(-53, -5786)};
    std::vector polyPoints2 = {Point2<long long>(-51, -5785), Point2<long long>(973, -5512),
                                                  Point2<long long>(973, -5939), Point2<long long>(-51, -5939)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 6);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge4() { //see polygonsUnionAlmostSameEdge4.ggb
    std::vector polyPoints1 = {Point2<long long>(-198, -6084), Point2<long long>(-214, -4811),
                                                  Point2<long long>(148, -4806), Point2<long long>(148, -5728)};
    std::vector polyPoints2 = {Point2<long long>(-199, -6084), Point2<long long>(-214, -4811), Point2<long long>(-203, -4811),
                                                  Point2<long long>(511, -4801), Point2<long long>(1061, -4795), Point2<long long>(417, -5454),
                                                  Point2<long long>(244, -5632)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 10);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge5() { //see polygonsUnionAlmostSameEdge5.ggb
    std::vector polyPoints1 = {Point2<long long>(829, -2960), Point2<long long>(1029, -2872), Point2<long long>(882, -3304)};
    std::vector polyPoints2 = {Point2<long long>(829, -2961), Point2<long long>(2379, -2275),
                                                  Point2<long long>(1994, -3397), Point2<long long>(896, -3397)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 7);
    //no points check
}

void PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge6() { //see polygonsUnionAlmostSameEdge6.ggb
    std::vector polyPoints1 = {Point2(-2.56617975f, -1.58268416f), Point2(2.56617975f, -1.58268416f),
                                              Point2(2.56617975f, -3.21731615f), Point2(-2.56617975f, -3.21731615f)};
    std::vector polyPoints2 = {Point2(55.3621445f, -75.836937f), Point2(42.3322601f, -75.836937f),
                                              Point2(-55.8955078f, 62.2369347f), Point2(-42.8656235f, 62.2369347f),
                                              Point2(0.222888947f, 1.66956329f), Point2(2.56617999f, 1.66957486f),
                                              Point2(2.56617999f, -1.62428343f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(55.3621445f, -75.836937f), Point2(42.3322601f, -75.836937f),
                                                                           Point2(-55.8955078f, 62.2369347f), Point2(-42.8656235f, 62.2369347f),
                                                                           Point2(0.222888947f, 1.66956329f), Point2(2.56617999f, 1.66957486f),
                                                                           Point2(2.56617999f, -1.58268416f), Point2(2.56617999f, -1.62428343f)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint1() {
    std::vector polyPoints1 = {Point2(0.0f, -1.0f), Point2(0.0f, 0.0f), Point2(2.0f, 1.0f)};
    std::vector polyPoints2 = {Point2(0.0f, 0.0f), Point2(0.0f, 1.0f), Point2(1.0f, 0.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, -1.0f), Point2(0.0f, 0.0f), Point2(0.0f, 1.0f),
                                                                           Point2(0.666666f, 0.333333f), Point2(2.0f, 1.0f)});
}

void PolygonsUnionTest::twoPolygonsTouchInOnePoint2() {
    std::vector polyPoints1 = {Point2(-30.9001007f, 17.9001007f), Point2(-30.5001011f, 17.8999023f),
                                              Point2(-30.6999035f, 17.5000992f)};

    std::vector polyPoints2 = {Point2(-30.9001007f, 17.9001007f), Point2(-30.7001019f, 18.5000992f),
                                              Point2(-30.6999035f, 17.9001007f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 2);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[0].getCwPoints().size(), 3);
    AssertHelper::assertUnsignedIntEquals(polygonUnion[1].getCwPoints().size(), 3);
    //no points check
}

void PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(0.0f, 2.0f), Point2(2.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(0.0f, 1.0f), Point2(0.5f, 1.0f), Point2(0.5f, 0.5f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.0f, 2.0f), Point2(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoIdenticalTriangles() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f), Point2(1.0f, 0.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.5f, 1.0f),
                                                                           Point2(1.0f, 0.0f)});
}

void PolygonsUnionTest::twoIdenticalSquares() {
    std::vector polyPoints1 = {Point2(-0.225821f, 5.97582f), Point2(1.12582f, 5.97582f),
                                             Point2(1.12582f, 4.62418f), Point2(-0.225821f, 4.62418f)};
    std::vector polyPoints2 = {Point2(-0.225821f, 5.97582f), Point2(1.12582f, 5.97582f),
                                             Point2(1.12582f, 4.62418f), Point2(-0.225821f, 4.62418f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(1.12582f, 4.62418f), Point2(-0.225821f, 4.62418f),
                                                                           Point2(-0.225821f, 5.97582f), Point2(1.12582f, 5.97582f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares1() {
    std::vector polyPoints1 = {Point2(-0.225821018f, -4.62417889f), Point2(1.12581992f, -4.62417889f),
                                              Point2(1.12581992f, -5.9758215f), Point2(-0.225821018f, -5.9758215f)};
    std::vector polyPoints2 = {Point2(-0.225821018f, -4.62417889f), Point2(1.12582099f, -4.62417889f),
                                              Point2(1.12582099f, -5.9758215f), Point2(-0.225821018f, -5.9758215f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(1.12582099f, -5.9758215f), Point2(-0.225821018f, -5.9758215f),
                                                                           Point2(-0.225821018f, -4.62417889f), Point2(1.12582099f, -4.62417889f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares2() {
    std::vector polyPoints1 = {Point2(1.0258255f, -5.87582397f), Point2(-0.125823975f, -5.87582397f),
                                              Point2(-0.125823975f, -4.72417879f), Point2(1.02582097f, -4.72417879f)};
    std::vector polyPoints2 = {Point2(1.02582097f, -5.87582159f), Point2(-0.125820994f, -5.87582159f),
                                              Point2(-0.125821024f, -4.72417879f), Point2(1.02582097f, -4.72417879f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(1.0258255f, -5.87582397f), Point2(-0.125823975f, -5.87582397f),
                                                                           Point2(-0.125823975f, -4.72417879f), Point2(1.02582097f, -4.72417879f)});
}

void PolygonsUnionTest::twoAlmostIdenticalSquares3() {
    std::vector polyPoints1 = {Point2(-0.175821006f, -4.67417908f), Point2(1.07582092f, -4.67417908f),
                                              Point2(1.07582092f, -5.9258213f), Point2(-0.175821006f, -5.9258213f)};
    std::vector polyPoints2 = {Point2(-0.175821006f, -4.67417908f), Point2(1.07581997f, -4.67417908f),
                                              Point2(1.07581997f, -5.9258213f), Point2(-0.175821006f, -5.9258213f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(1.07582f, -5.9258213f), Point2(-0.175821006f, -5.9258213f),
                                                                           Point2(-0.175821006f, -4.67417908f), Point2(1.07582092f, -4.67417908f)});
}

void PolygonsUnionTest::twoAlmostIdenticalTrapeze() { //see polygonsUnionAlmostIdenticalTrapeze.ggb
    std::vector polyPoints1 = {Point2<long long>(748, -4979), Point2<long long>(922, -5010),
                                                  Point2<long long>(922, -5786), Point2<long long>(541, -5786)};
    std::vector polyPoints2 = {Point2<long long>(747, -4979), Point2<long long>(973, -5019),
                                                  Point2<long long>(973, -5939), Point2<long long>(502, -5939)};

    std::vector<CSGPolygon<long long>> allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2))};
    std::vector<CSGPolygon<long long>> polygonUnion = PolygonsUnion<long long>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonLongLongEquals(polygonUnion[0].getCwPoints(), {Point2<long long>(973, -5939), Point2<long long>(502, -5939), Point2<long long>(747, -4979),
                                                                              Point2<long long>(748, -4979), Point2<long long>(922, -5010), Point2<long long>(973, -5019)});
}

void PolygonsUnionTest::threePolygonsUnion() {
    std::vector polyPoints1 = {Point2(0.0f, 0.0f), Point2(0.0f, 2.0f),
                                              Point2(2.0f, 2.0f), Point2(2.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(0.5f, 1.0f), Point2(1.0f, 3.0f), Point2(1.5f, 1.0f)};
    std::vector polyPoints3 = {Point2(1.0f, 0.5f), Point2(1.0f, 1.5f), Point2(3.0f, 1.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2)), CSGPolygon("p3", std::move(polyPoints3))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 1);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(0.0f, 0.0f), Point2(0.0f, 2.0f), Point2(0.75f, 2.0f),
                                                                           Point2(1.0f, 3.0f), Point2(1.25f, 2.0f), Point2(2.0f, 2.0f),
                                                                           Point2(2.0f, 1.25f), Point2(3.0f, 1.0f), Point2(2.0f, 0.75f),
                                                                           Point2(2.0f, 0.0f)});
}

void PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon() {
    std::vector polyPoints1 = {Point2(4.0f, 0.0f), Point2(4.0f, 2.0f),
                                              Point2(6.0f, 2.0f), Point2(6.0f, 0.0f)};
    std::vector polyPoints2 = {Point2(0.5f, 1.0f), Point2(1.0f, 3.0f), Point2(1.5f, 1.0f)};
    std::vector polyPoints3 = {Point2(1.0f, 0.5f), Point2(1.0f, 1.5f), Point2(3.0f, 1.0f)};

    std::vector allPolygons = {CSGPolygon("p1", std::move(polyPoints1)), CSGPolygon("p2", std::move(polyPoints2)), CSGPolygon("p3", std::move(polyPoints3))};
    std::vector polygonUnion = PolygonsUnion<float>::instance().unionPolygons(allPolygons);

    AssertHelper::assertUnsignedIntEquals(polygonUnion.size(), 2);
    AssertHelper::assertPolygonFloatEquals(polygonUnion[0].getCwPoints(), {Point2(4.0f, 0.0f), Point2(4.0f, 2.0f), Point2(6.0f, 2.0f),
                                                                           Point2(6.0f, 0.0f)});
    AssertHelper::assertPolygonFloatEquals(polygonUnion[1].getCwPoints(), {Point2(1.0f, 0.5f), Point2(1.0f, 1.0f), Point2(0.5f, 1.0f),
                                                                           Point2(1.0f, 3.0f), Point2(1.4f, 1.4f), Point2(3.0f, 1.0f)});
}

void PolygonsUnionTest::roomPolygonsUnion() {
    std::vector frontLeftWallPolyPoints = {Point2(-11.875f, -0.875f), Point2(-9.875f, -0.875f), Point2(-9.875f, -2.125f), Point2(-11.875f, -2.125f)};
    std::vector frontDoorPolyPoints = {Point2(-10.375f, -1.14999998f), Point2(-7.625f, -1.14999998f), Point2(-7.625f, -1.85000002f), Point2(-10.375f, -1.85000002f)};
    std::vector rightWallPolyPoints = {Point2(-7.37499905f, 2.875f), Point2(-6.12499905f, 2.87499952f), Point2(-6.12500048f, -1.37500238f), Point2(-7.37500048f, -1.37500191f)};
    std::vector frontRightWallPolyPoints = {Point2(-8.125f, -0.875f), Point2(-6.125f, -0.875f), Point2(-6.125f, -2.125f), Point2(-8.125f, -2.125f)};
    std::vector backWallPolyPoints = {Point2(-11.125001f, 3.625f), Point2(-6.87499905f, 3.625f), Point2(-6.87499905f, 2.375f), Point2(-11.125001f, 2.375f)};
    std::vector leftWallPolyPoints = {Point2(-11.8749971f, 2.87500024f), Point2(-10.6249971f, 2.87499976f), Point2(-10.625f, -1.37500238f), Point2(-11.875f, -1.37500191f)};
    std::vector leftCornerWallPolyPoints = {Point2(-12.0602951f, 3.73862982f), Point2(-11.7788267f, 4.02009869f), Point2(-10.624999f, 4.12500286f), Point2(-12.375001f, 2.37499976f)};

    std::vector allPolygons = {
            CSGPolygon("frontLeftWall", std::move(frontLeftWallPolyPoints)),
            CSGPolygon("frontDoor", std::move(frontDoorPolyPoints)),
            CSGPolygon("rightWall", std::move(rightWallPolyPoints)),
            CSGPolygon("frontRightWall", std::move(frontRightWallPolyPoints)),
            CSGPolygon("backWall", std::move(backWallPolyPoints)),
            CSGPolygon("leftWall", std::move(leftWallPolyPoints)),
            CSGPolygon("leftCornerWall", std::move(leftCornerWallPolyPoints))
    };
    PolygonsUnion<float>::instance().unionPolygons(allPolygons); //TO DO: bug to fix (make crash commented line in CSGPolygon#CSGPolygon())

    //no crash check
}

CppUnit::Test* PolygonsUnionTest::suite() {
    auto* suite = new CppUnit::TestSuite("PolygonsUnionTest");

    suite->addTest(new CppUnit::TestCaller("onePolygonUnion", &PolygonsUnionTest::onePolygonUnion));

    suite->addTest(new CppUnit::TestCaller("twoPolygonsNoUnion", &PolygonsUnionTest::twoPolygonsNoUnion));
    suite->addTest(new CppUnit::TestCaller("polygonInsideAnother", &PolygonsUnionTest::polygonInsideAnother));
    suite->addTest(new CppUnit::TestCaller("polygonInsideAnotherTouchInOnePoint", &PolygonsUnionTest::polygonInsideAnotherTouchInOnePoint));

    suite->addTest(new CppUnit::TestCaller("twoPolygonsWithHoleInside", &PolygonsUnionTest::twoPolygonsWithHoleInside));

    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnion", &PolygonsUnionTest::twoPolygonsUnion));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionXAligned", &PolygonsUnionTest::twoPolygonsUnionXAligned));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionYAligned", &PolygonsUnionTest::twoPolygonsUnionYAligned));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionYAlmostAligned", &PolygonsUnionTest::twoPolygonsUnionYAlmostAligned));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge1", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge1));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge2", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge2));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge3", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge3));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge4", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge4));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge5", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge5));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAlmostSameEdge6", &PolygonsUnionTest::twoPolygonsUnionAlmostSameEdge6));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsTouchInOnePoint1", &PolygonsUnionTest::twoPolygonsTouchInOnePoint1));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsTouchInOnePoint2", &PolygonsUnionTest::twoPolygonsTouchInOnePoint2));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsNestedTouchInOnePoint", &PolygonsUnionTest::twoPolygonsNestedTouchInOnePoint));

    suite->addTest(new CppUnit::TestCaller("twoIdenticalTriangles", &PolygonsUnionTest::twoIdenticalTriangles));
    suite->addTest(new CppUnit::TestCaller("twoIdenticalSquares", &PolygonsUnionTest::twoIdenticalSquares));
    suite->addTest(new CppUnit::TestCaller("twoAlmostIdenticalSquares1", &PolygonsUnionTest::twoAlmostIdenticalSquares1));
    suite->addTest(new CppUnit::TestCaller("twoAlmostIdenticalSquares2", &PolygonsUnionTest::twoAlmostIdenticalSquares2));
    suite->addTest(new CppUnit::TestCaller("twoAlmostIdenticalSquares3", &PolygonsUnionTest::twoAlmostIdenticalSquares3));
    suite->addTest(new CppUnit::TestCaller("twoAlmostIdenticalTrapeze", &PolygonsUnionTest::twoAlmostIdenticalTrapeze));

    suite->addTest(new CppUnit::TestCaller("threePolygonsUnion", &PolygonsUnionTest::threePolygonsUnion));
    suite->addTest(new CppUnit::TestCaller("twoPolygonsUnionAndSeparatePolygon", &PolygonsUnionTest::twoPolygonsUnionAndSeparatePolygon));

    suite->addTest(new CppUnit::TestCaller("roomPolygonsUnion", &PolygonsUnionTest::roomPolygonsUnion));

    return suite;
}
