#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinAIEngine.h"

#include "TerrainObstacleServiceTest.h"
#include "AssertHelper.h"
using namespace urchin;

void TerrainObstacleServiceTest::oneSquare() {
    std::vector<Point3<float>> localVertices = {
            Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(2.0, 100.0, 0.0),
            Point3<float>(0.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0), Point3<float>(2.0, 0.0, 1.0),
            Point3<float>(0.0, 0.0, 2.0), Point3<float>(1.0, 0.0, 2.0), Point3<float>(2.0, 0.0, 2.0)
    };
    TerrainObstacleService terrainObstacleService("terrain", Point3<float>(0.0, 0.0, 0.0), localVertices, 3, 3);

    std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(0.01);

    AssertHelper::assertUnsignedInt(selfObstacles.size(), 1);
    AssertHelper::assertTrue(selfObstacles[0].getName()=="terrain_obstacle0");
    AssertHelper::assertUnsignedInt(selfObstacles[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[0], Point2<float>(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[1], Point2<float>(2.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[2], Point2<float>(2.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[3], Point2<float>(1.0f, -1.0f));
}

void TerrainObstacleServiceTest::twoAlignedSquares() {
    std::vector<Point3<float>> localVertices = {
            Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(2.0, 100.0, 0.0),
            Point3<float>(0.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0), Point3<float>(2.0, 100.0, 1.0),
            Point3<float>(0.0, 0.0, 2.0), Point3<float>(1.0, 0.0, 2.0), Point3<float>(2.0, 0.0, 2.0)
    };
    TerrainObstacleService terrainObstacleService("terrain", Point3<float>(0.0, 0.0, 0.0), localVertices, 3, 3);

    std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(0.01);

    AssertHelper::assertUnsignedInt(selfObstacles.size(), 1);
    AssertHelper::assertTrue(selfObstacles[0].getName()=="terrain_obstacle0");
    AssertHelper::assertUnsignedInt(selfObstacles[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[0], Point2<float>(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[1], Point2<float>(2.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[2], Point2<float>(2.0f, -2.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[3], Point2<float>(1.0f, -2.0f));
}

void TerrainObstacleServiceTest::twoSquaresSamePoint() {
    std::vector<Point3<float>> localVertices = {
            Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(2.0, 100.0, 0.0),
            Point3<float>(0.0, 0.0, 1.0), Point3<float>(1.0, 0.0, 1.0), Point3<float>(2.0, 0.0, 1.0),
            Point3<float>(0.0, 100.0, 2.0), Point3<float>(1.0, 0.0, 2.0), Point3<float>(2.0, 0.0, 2.0)
    };
    TerrainObstacleService terrainObstacleService("terrain", Point3<float>(0.0, 0.0, 0.0), localVertices, 3, 3);

    std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(0.01);

    AssertHelper::assertUnsignedInt(selfObstacles.size(), 2);
    AssertHelper::assertTrue(selfObstacles[0].getName()=="terrain_obstacle0");
    AssertHelper::assertUnsignedInt(selfObstacles[0].getCwPoints().size(), 4);
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[0], Point2<float>(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[1], Point2<float>(2.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[2], Point2<float>(2.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[3], Point2<float>(1.0f, -1.0f));
    AssertHelper::assertUnsignedInt(selfObstacles[1].getCwPoints().size(), 4);
    AssertHelper::assertTrue(selfObstacles[1].getName()=="terrain_obstacle1");
    AssertHelper::assertPoint2FloatEquals(selfObstacles[1].getCwPoints()[0], Point2<float>(0.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[1].getCwPoints()[1], Point2<float>(1.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[1].getCwPoints()[2], Point2<float>(1.0f, -2.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[1].getCwPoints()[3], Point2<float>(0.0f, -2.0f));
}

void TerrainObstacleServiceTest::squaresInUForm() {
    std::vector<Point3<float>> localVertices = {
            Point3<float>(0.0, 0.0, 0.0), Point3<float>(1.0, 0.0, 0.0), Point3<float>(2.0, 0.0, 0.0), Point3<float>(3.0, 0.0, 0.0),
            Point3<float>(0.0, 100.0, 1.0), Point3<float>(1.0, 0.0, 1.0), Point3<float>(2.0, 0.0, 1.0), Point3<float>(3.0, 100.0, 1.0),
            Point3<float>(0.0, 100.0, 2.0), Point3<float>(1.0, 100.0, 2.0), Point3<float>(2.0, 100.0, 2.0), Point3<float>(3.0, 100.0, 2.0)
    };
    TerrainObstacleService terrainObstacleService("terrain", Point3<float>(0.0, 0.0, 0.0), localVertices, 4, 3);

    std::vector<CSGPolygon<float>> selfObstacles = terrainObstacleService.computeSelfObstacles(0.01);

    AssertHelper::assertUnsignedInt(selfObstacles.size(), 1);
    AssertHelper::assertTrue(selfObstacles[0].getName()=="terrain_obstacle0");
    AssertHelper::assertUnsignedInt(selfObstacles[0].getCwPoints().size(), 8);
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[0], Point2<float>(0.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[1], Point2<float>(1.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[2], Point2<float>(1.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[3], Point2<float>(2.0f, -1.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[4], Point2<float>(2.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[5], Point2<float>(3.0f, 0.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[6], Point2<float>(3.0f, -2.0f));
    AssertHelper::assertPoint2FloatEquals(selfObstacles[0].getCwPoints()[7], Point2<float>(0.0f, -2.0f));
}

CppUnit::Test *TerrainObstacleServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("TerrainObstacleServiceTest");

    suite->addTest(new CppUnit::TestCaller<TerrainObstacleServiceTest>("oneSquare", &TerrainObstacleServiceTest::oneSquare));
    suite->addTest(new CppUnit::TestCaller<TerrainObstacleServiceTest>("twoAlignedSquares", &TerrainObstacleServiceTest::twoAlignedSquares));
    suite->addTest(new CppUnit::TestCaller<TerrainObstacleServiceTest>("twoSquaresSamePoint", &TerrainObstacleServiceTest::twoSquaresSamePoint));
    suite->addTest(new CppUnit::TestCaller<TerrainObstacleServiceTest>("squaresInUForm", &TerrainObstacleServiceTest::squaresInUForm));

    return suite;
}
