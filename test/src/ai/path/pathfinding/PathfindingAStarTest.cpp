#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "PathfindingAStarTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PathfindingAStarTest::straightPath()
{
    std::vector<Point3<float>> polygonPoints = {Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(0.0f, 0.0f, 4.0f), Point3<float>(4.0f, 0.0f, 4.0f), Point3<float>(4.0f, 0.0f, 0.0f)};
    auto navPolygon = std::make_shared<NavPolygon>("polyTestName", std::move(polygonPoints), nullptr);
    auto navTriangle1 = std::make_shared<NavTriangle>(0, 1, 3);
    auto navTriangle2 = std::make_shared<NavTriangle>(1, 2, 3);
    navPolygon->addTriangles({navTriangle1, navTriangle2}, navPolygon);

    navTriangle1->addDirectLink(1, navTriangle2);
    auto navMesh = std::make_shared<NavMesh>();
    navMesh->copyAllPolygons({navPolygon});
    PathfindingAStar pathfindingAStar(navMesh);

    std::vector<PathPoint> pathPoints = pathfindingAStar.findPath(Point3<float>(1.0f, 0.0f, 1.0f), Point3<float>(3.0f, 0.0f, 3.0f));

    AssertHelper::assertUnsignedInt(pathPoints.size(), 2);
    AssertHelper::assertPoint3FloatEquals(pathPoints[0].getPoint(), Point3<float>(1.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(pathPoints[1].getPoint(), Point3<float>(3.0f, 0.0f, 3.0f));
}

void PathfindingAStarTest::jumpWithSmallConstraint()
{
    std::vector<PathPoint> pathPoints = pathWithJump(new NavLinkConstraint(1.0f, 0.0f, 2));

    AssertHelper::assertUnsignedInt(pathPoints.size(), 4);
    AssertHelper::assertPoint3FloatEquals(pathPoints[0].getPoint(), Point3<float>(1.0f, 0.0f, 1.0f));
    AssertHelper::assertTrue(!pathPoints[0].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[1].getPoint(), Point3<float>(2.0f, 0.0f, 2.0f)); //info: not the best point due to simplification in PathfindingAStar::computeTransitionPoint
    AssertHelper::assertTrue(pathPoints[1].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[2].getPoint(), Point3<float>(2.5f, 0.0f, 2.5f)); //info: not the best point due to simplification in PathfindingAStar::computeTransitionPoint
    AssertHelper::assertTrue(!pathPoints[2].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[3].getPoint(), Point3<float>(3.0f, 0.0f, 4.0f));
    AssertHelper::assertTrue(!pathPoints[3].isJumpPoint());
}

void PathfindingAStarTest::jumpWithBigConstraint()
{
    std::vector<PathPoint> pathPoints = pathWithJump(new NavLinkConstraint(0.01f, 0.0f, 2));

    AssertHelper::assertUnsignedInt(pathPoints.size(), 4);
    AssertHelper::assertPoint3FloatEquals(pathPoints[0].getPoint(), Point3<float>(1.0f, 0.0f, 1.0f));
    AssertHelper::assertTrue(!pathPoints[0].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[1].getPoint(), Point3<float>(3.96f, 0.0f, 0.04f));
    AssertHelper::assertTrue(pathPoints[1].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[2].getPoint(), Point3<float>(4.0f, 0.0f, 1.0f));
    AssertHelper::assertTrue(!pathPoints[2].isJumpPoint());
    AssertHelper::assertPoint3FloatEquals(pathPoints[3].getPoint(), Point3<float>(3.0f, 0.0f, 4.0f));
    AssertHelper::assertTrue(!pathPoints[3].isJumpPoint());
}

std::vector<PathPoint> PathfindingAStarTest::pathWithJump(NavLinkConstraint *navLinkConstraint)
{
    std::vector<Point3<float>> polygon1Points = {Point3<float>(0.0f, 0.0f, 0.0f), Point3<float>(0.0f, 0.0f, 4.0f), Point3<float>(4.0f, 0.0f, 0.0f)};
    auto navPolygon1 = std::make_shared<NavPolygon>("poly1TestName", std::move(polygon1Points), nullptr);
    auto navPolygon1Triangle1 = std::make_shared<NavTriangle>(0, 1, 2);
    navPolygon1->addTriangles({navPolygon1Triangle1}, navPolygon1);

    std::vector<Point3<float>> polygon2Points = {Point3<float>(0.0f, 0.0f, 5.0f), Point3<float>(4.0f, 0.0f, 5.0f), Point3<float>(4.0f, 0.0f, 1.0f)};
    auto navPolygon2 = std::make_shared<NavPolygon>("poly2TestName", std::move(polygon2Points), nullptr);
    auto navPolygon2Triangle1 = std::make_shared<NavTriangle>(0, 1, 2);
    navPolygon2->addTriangles({navPolygon2Triangle1}, navPolygon2);

    navPolygon1Triangle1->addJumpLink(1, navPolygon2Triangle1, navLinkConstraint);
    auto navMesh = std::make_shared<NavMesh>();
    navMesh->copyAllPolygons({navPolygon1, navPolygon2});
    PathfindingAStar pathfindingAStar(navMesh);

    return pathfindingAStar.findPath(Point3<float>(1.0f, 0.0f, 1.0f), Point3<float>(3.0f, 0.0f, 4.0f));
}

CppUnit::Test *PathfindingAStarTest::suite()
{
    auto *suite = new CppUnit::TestSuite("PathfindingAStarTest");

    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("straightPath", &PathfindingAStarTest::straightPath));
    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("jumpWithSmallConstraint", &PathfindingAStarTest::jumpWithSmallConstraint));
    suite->addTest(new CppUnit::TestCaller<PathfindingAStarTest>("jumpWithBigConstraint", &PathfindingAStarTest::jumpWithBigConstraint));

    return suite;
}
