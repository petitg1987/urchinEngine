#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include "UrchinCommon.h"

#include "math/geometry/ResizePolygon2DServiceTest.h"
#include "AssertHelper.h"
using namespace urchin;

void ResizePolygon2DServiceTest::reduceTriangle()
{
    std::vector<Point2<int>> trianglesPoints = {Point2<int>(-188, -6065), Point2<int>(1100, -6065), Point2<int>(1100, -4774)};

    std::vector<Point2<int>> polygonResult = ResizePolygon2DService<int>::instance()->resizePolygon(trianglesPoints, -5);

    AssertHelper::assertPoint2IntEquals(polygonResult[0], Point2<int>(-176, -6060));
    AssertHelper::assertPoint2IntEquals(polygonResult[1], Point2<int>(1095, -6060));
    AssertHelper::assertPoint2IntEquals(polygonResult[2], Point2<int>(1095, -4786));
}

void ResizePolygon2DServiceTest::reduceWithEdgesAcceptance()
{
    std::vector<Point2<float>> polygonPoints = {Point2<float>(0.0, 4.0), Point2<float>(1.0, 4.0), Point2<float>(1.0, 3.0),
                                                Point2<float>(3.0, 3.0), Point2<float>(3.0, 4.0), Point2<float>(4.0, 4.0),
                                                Point2<float>(4.0, 0.0), Point2<float>(0.0, 0.0)};
    std::vector<bool> edgesAcceptance = {true, true, false,
                                        false, true, true,
                                        true, true};

    std::vector<Point2<float>> polygonResult = ResizePolygon2DService<float>::instance()->resizePolygon(polygonPoints, -0.5, edgesAcceptance);

    AssertHelper::assertPoint2FloatEquals(polygonResult[0], Point2<float>(-0.5, 4.5));
    AssertHelper::assertPoint2FloatEquals(polygonResult[1], Point2<float>(1.0, 4.5));
    AssertHelper::assertPoint2FloatEquals(polygonResult[2], Point2<float>(1.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonResult[3], Point2<float>(3.0, 3.0));
    AssertHelper::assertPoint2FloatEquals(polygonResult[4], Point2<float>(3.0, 4.5));
    AssertHelper::assertPoint2FloatEquals(polygonResult[5], Point2<float>(4.5, 4.5));
    AssertHelper::assertPoint2FloatEquals(polygonResult[6], Point2<float>(4.5, -0.5));
    AssertHelper::assertPoint2FloatEquals(polygonResult[7], Point2<float>(-0.5, -0.5));
}

CppUnit::Test *ResizePolygon2DServiceTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("ResizePolygon2DServiceTest");

    suite->addTest(new CppUnit::TestCaller<ResizePolygon2DServiceTest>("reduceTriangle", &ResizePolygon2DServiceTest::reduceTriangle));
    suite->addTest(new CppUnit::TestCaller<ResizePolygon2DServiceTest>("reduceWithEdgesAcceptance", &ResizePolygon2DServiceTest::reduceWithEdgesAcceptance));

    return suite;
}
