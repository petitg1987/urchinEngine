#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "PolygonsSubtractionTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PolygonsSubtractionTest::polygonsSubtractionMiddlePoint()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(1.0, 2.0), Point2<float>(5.0, 2.0), Point2<float>(6.0, 1.0),
                                              Point2<float>(5.0, 0.0), Point2<float>(1.0, 0.0), Point2<float>(0.0, 1.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(2.0, 3.0), Point2<float>(4.5, 1.5), Point2<float>(4.0, -1.0),
                                                 Point2<float>(3.0, 1.0), Point2<float>(2.0, -1.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    //TODO check results
}

void PolygonsSubtractionTest::polygonsSubtractionSameEdge()
{
    std::vector<Point2<float>> minuendPoly = {Point2<float>(0.0, 4.0), Point2<float>(4.0, 4.0),
                                              Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};

    std::vector<Point2<float>> subtrahendPoly = {Point2<float>(1.0, 5.0), Point2<float>(3.0, 5.0), Point2<float>(3.0, 3.0),
                                                 Point2<float>(2.0, 3.0), Point2<float>(2.0, 4.0), Point2<float>(1.0, 4.0)};

    std::vector<CSGPolygon<float>> polygonSubtraction = PolygonsSubtraction<float>::instance()->subtractPolygons(
            CSGPolygon<float>("minuend", minuendPoly), CSGPolygon<float>("subtrahend", subtrahendPoly));

    //TODO check results
}

CppUnit::Test *PolygonsSubtractionTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("PolygonsSubtractionTest");

    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionMiddlePoint", &PolygonsSubtractionTest::polygonsSubtractionMiddlePoint));
    suite->addTest(new CppUnit::TestCaller<PolygonsSubtractionTest>("polygonsSubtractionSameEdge", &PolygonsSubtractionTest::polygonsSubtractionSameEdge));

    return suite;
}
