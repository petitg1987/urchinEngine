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

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
    std::vector<bool> isMinuendPoints0 = {true, true, true, true};
    AssertHelper::assertTrue(isMinuendPoints[0]==isMinuendPoints0);
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactPoint()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(4.0, 3.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
    std::vector<bool> isMinuendPoints0 = {true, true, true, true};
    AssertHelper::assertTrue(isMinuendPoints[0]==isMinuendPoints0);
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactEdge()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 3.0), Point2<float>(4.0, 3.0),
                                                 Point2<float>(4.0, 1.0), Point2<float>(1.0, 1.0)};

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
    std::vector<bool> isMinuendPoints0 = {true, true, true, true};
    AssertHelper::assertTrue(isMinuendPoints[0]==isMinuendPoints0);
}

void PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 7.0), Point2<float>(3.0, 7.0),
                                                 Point2<float>(3.0, 5.0), Point2<float>(1.0, 5.0)};

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.0, 0.0));
    std::vector<bool> isMinuendPoints0 = {true, true, true, true};
    AssertHelper::assertTrue(isMinuendPoints[0]==isMinuendPoints0);
}

void PolygonsSubtractionTest::polygonsSubtractionMinuendInside()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 3.0), Point2<float>(3.0, 3.0),
                                              Point2<float>(3.0, 1.0), Point2<float>(1.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                                 Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 0);
    AssertHelper::assertUnsignedInt(isMinuendPoints.size(), 0);
}

void PolygonsSubtractionTest::polygonsSubtractionMiddlePoint()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 2.0), Point2<float>(5.0, 2.0), Point2<float>(6.0, 1.0),
                                              Point2<float>(5.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.0, 3.0), Point2<float>(4.5, 1.5), Point2<float>(4.0, -1.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(2.0, -1.0)};

    bool subtrahendInsideMinuend;
    std::map<unsigned int, std::vector<bool>> isMinuendPoints;
    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly), subtrahendInsideMinuend, isMinuendPoints);

    AssertHelper::assertTrue(!subtrahendInsideMinuend);
    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 3);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 5);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(1.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(2.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(2.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(1.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(0.0, 1.0));
    std::vector<bool> isMinuendPoints0 = {true, true, true, true, true};
    AssertHelper::assertTrue(isMinuendPoints[0]==isMinuendPoints0);
    AssertHelper::assertUnsignedInt(polygonSubtraction[1].getCwPoints().size(), 6);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[0], Point2<float>(5.0, 2.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[1], Point2<float>(6.0, 1.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[2], Point2<float>(5.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[3], Point2<float>(4.2, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[4], Point2<float>(4.5, 1.5));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[5], Point2<float>(3.666666, 2.0));
    std::vector<bool> isMinuendPoints1 = {true, true, true, true, false, true};
    AssertHelper::assertTrue(isMinuendPoints[1]==isMinuendPoints1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[2].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[0], Point2<float>(3.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[1], Point2<float>(2.5, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[2], Point2<float>(3.0, 1.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[2].getCwPoints()[3], Point2<float>(3.5, 0.0));
    std::vector<bool> isMinuendPoints2 = {true, true, false, true};
    AssertHelper::assertTrue(isMinuendPoints[2]==isMinuendPoints2);
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

void PolygonsSubtractionTest::polygonsSubtractionMaxIteration()
{ //see polygonsSubtractionMaxIteration.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.724967539, -6.26921654), Point2<float>(2.0384562, -6.03173637),
                                              Point2<float>(1.0561136, -8.09460735)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(0.756835938, -6.38183594), Point2<float>(1.75683594, -6.38183594),
                                                 Point2<float>(1.75683594, -6.43554688), Point2<float>(0.756835938, -6.61425781)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 8);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(0.724967539, -6.26921654));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(2.0384562, -6.03173637));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(1.0561136, -8.09460735));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(0.786597, -6.608939));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(1.75683594, -6.43554688));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(1.75683594, -6.38183594));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(0.756835938, -6.38183594));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(0.756836, -6.444886));
}

void PolygonsSubtractionTest::polygonsSubtractionTwoIntersectionsOnEdge()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(-0.00646264944, -6.47982645), Point2<float>(1.59619427, -4.96815157),
                                              Point2<float>(1.53988385, -6.4921689)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.45019531, -4.84667969), Point2<float>(2.71777344, -6.58496094),
                                                 Point2<float>(0.471679688, -6.90820312), Point2<float>(0.204101562, -5.16992188)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 2);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 3);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(-0.00646264944, -6.47982645));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(0.353478, -6.140319));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(0.406246, -6.483120));
    AssertHelper::assertUnsignedInt(polygonSubtraction[1].getCwPoints().size(), 3);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[0], Point2<float>(1.59619427, -4.96815157));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[1], Point2<float>(1.596141, -4.969589));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[1].getCwPoints()[2], Point2<float>(1.594405, -4.969839));
}

void PolygonsSubtractionTest::polygonsSubtractionCorner1()
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

void PolygonsSubtractionTest::polygonsSubtractionCorner2()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(-1.0, 2.0), Point2<float>(2.0, 15.0),
                                                 Point2<float>(5.0, 2.0), Point2<float>(2.0, 5.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 6);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(4.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(0.0, 0.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(0.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(1.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(3.0, 4.0));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(4.0, 3.0));
}

void PolygonsSubtractionTest::polygonsSubtractionCorner3()
{ //see polygonsSubtractionCorner3.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755, -8.64567757), Point2<float>(4.47754383, -7.34843636),
                                              Point2<float>(5.91046381, -6.30504799), Point2<float>(7.27159405, -7.60229063)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0, -6.0), Point2<float>(7.44508, -6.18886),
                                                 Point2<float>(4.98730469, -6.67675781), Point2<float>(5.31445312, -7.43554688)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 9);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(5.8386755, -8.64567757));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.47754383, -7.34843636));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.93181419, -7.01765728));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(5.31445312, -7.43554688));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(5.08590603, -6.90545416));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(5.32028055, -6.73479319));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(5.55465508, -6.56413221));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[7], Point2<float>(6.07411051, -6.46101379));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[8], Point2<float>(7.27159405, -7.60229063));
}

void PolygonsSubtractionTest::polygonsSubtractionCorner4()
{ //see polygonsSubtractionCorner4.ggb
    std::vector<Point2<float>> minuendPoly = {Point2<float>(5.8386755, -8.64567757), Point2<float>(4.47754383, -7.34843636),
                                              Point2<float>(5.91046381, -6.30504799), Point2<float>(7.27159405, -7.60229063)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(4.0, -6.0), Point2<float>(70.5371094, 20.34375),
                                                 Point2<float>(4.98730469, -6.67675781), Point2<float>(5.31445312, -7.43554688)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    AssertHelper::assertUnsignedInt(polygonSubtraction.size(), 1);
    AssertHelper::assertUnsignedInt(polygonSubtraction[0].getCwPoints().size(), 7);
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[0], Point2<float>(5.8386755, -8.64567757));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[1], Point2<float>(4.47754383, -7.34843636));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[2], Point2<float>(4.93181419, -7.01765728));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[3], Point2<float>(5.31445312, -7.43554688));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[4], Point2<float>(5.08590603, -6.90545416));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[5], Point2<float>(5.91046381, -6.30504799));
    AssertHelper::assertPoint2FloatEquals(polygonSubtraction[0].getCwPoints()[6], Point2<float>(7.27159405, -7.60229063));
}

CppUnit::Test *PolygonsSubtractionTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsSubtractionTest");

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInsideContactPoint", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactPoint));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendInsideContactEdge", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendInsideContactEdge));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSubtrahendOutside", &PolygonsSubtractionTest::polygonsSubtractionSubtrahendOutside));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMinuendInside", &PolygonsSubtractionTest::polygonsSubtractionMinuendInside));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMiddlePoint", &PolygonsSubtractionTest::polygonsSubtractionMiddlePoint));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionSameEdge2));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge1", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge2", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge3", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoSameEdge4", &PolygonsSubtractionTest::polygonsSubtractionTwoSameEdge4));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMaxIteration", &PolygonsSubtractionTest::polygonsSubtractionMaxIteration));

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionTwoIntersectionsOnEdge", &PolygonsSubtractionTest::polygonsSubtractionTwoIntersectionsOnEdge));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner1", &PolygonsSubtractionTest::polygonsSubtractionCorner1));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner2", &PolygonsSubtractionTest::polygonsSubtractionCorner2));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner3", &PolygonsSubtractionTest::polygonsSubtractionCorner3));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionCorner4", &PolygonsSubtractionTest::polygonsSubtractionCorner4));

    return suite;
}
