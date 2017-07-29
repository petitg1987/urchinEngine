#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsSubtractionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 7.0), Point2<float>(3.0, 7.0),
                                                 Point2<float>(3.0, 5.0), Point2<float>(1.0, 5.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionMiddlePoint()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 2.0), Point2<float>(5.0, 2.0), Point2<float>(6.0, 1.0),
                                              Point2<float>(5.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.0, 3.0), Point2<float>(4.5, 1.5), Point2<float>(4.0, -1.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(2.0, -1.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 3);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 5);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(1.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(2.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(2.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(0.0, 1.0));
    AssertHelper::assertUnsignedInt(polygonSubtraction[1].getCwPoints().size(), 6);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[0], Point2<float>(5.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[1], Point2<float>(6.0, 1.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[2], Point2<float>(5.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[3], Point2<float>(4.2, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[4], Point2<float>(4.5, 1.5));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[5], Point2<float>(3.666666, 2.0));
    AssertHelper::assertUnsignedInt(polygonSubtraction[2].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[0], Point2<float>(3.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[1], Point2<float>(2.5, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[2], Point2<float>(3.0, 1.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[3], Point2<float>(3.5, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge1()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(2.0, 3.0), Point2<float>(2.0, 4.0), Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 9);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(2.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(2.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(3.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[8], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge2()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(2.0, 3.0), Point2<float>(2.0, 4.0), Point2<float>(0.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 8);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(0.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(2.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(2.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(3.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(3.0, 4.0));
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0), Point2<float>(2.0, 3.0),
                                              Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0),
                                              Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 9);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(1.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(2.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(2.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[8], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(1.5, 4.0), Point2<float>(2.0, 4.5),
                                              Point2<float>(2.5, 4.0), Point2<float>(4.0, 4.0), Point2<float>(4.0, 0.0),
                                              Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 8);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(1.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(2.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(2.5, 4.0), Point2<float>(2.0, 3.0), Point2<float>(1.5, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 9);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(1.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(2.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(2.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[8], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 4.0),
                                                 Point2<float>(2.5, 4.0), Point2<float>(2.0, 4.5), Point2<float>(1.5, 4.0),
                                                 Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 9);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(1.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(2.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(2.5, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[8], Point2<float>(0.0, 0.0));
}

void PolygonsSubtractionTest::polygonsSubtractionCorner()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(3.0, 4.0), Point2<float>(3.0, 5.0), Point2<float>(5.0, 5.0),
                                                 Point2<float>(5.0, 3.0), Point2<float>(4.0, 3.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 5);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(0.0, 0.0));
}

CppUnit::Test *PolygonsSubtractionTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsSubtractionTest");

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendOutside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMiddlePoint", &PolygonsSubtractionTest::polygonsSubtractionMiddlePoint));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionSameEdge2));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge3", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge4", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner", &PolygonsSubtractionTest::polygonsSubtractionCorner));

    return suite;
}
