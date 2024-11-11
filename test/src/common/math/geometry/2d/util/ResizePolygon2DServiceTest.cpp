#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <vector>
#include <UrchinCommon.h>

#include <common/math/geometry/2d/util/ResizePolygon2DServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void ResizePolygon2DServiceTest::reduceTriangle() {
    std::vector trianglesPoints = {Point2(-188, -6065), Point2(1100, -6065), Point2(1100, -4774)};

    ResizePolygon2DService<int>::resizePolygon(trianglesPoints, -5);

    AssertHelper::assertPoint2IntEquals(trianglesPoints[0], Point2<int>(-176, -6060));
    AssertHelper::assertPoint2IntEquals(trianglesPoints[1], Point2<int>(1095, -6060));
    AssertHelper::assertPoint2IntEquals(trianglesPoints[2], Point2<int>(1095, -4786));
}

CppUnit::Test* ResizePolygon2DServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("ResizePolygon2DServiceTest");

    suite->addTest(new CppUnit::TestCaller("reduceTriangle", &ResizePolygon2DServiceTest::reduceTriangle));

    return suite;
}
