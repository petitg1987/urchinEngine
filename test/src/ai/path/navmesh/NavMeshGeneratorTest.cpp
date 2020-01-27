#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"

#include "NavMeshGeneratorTest.h"
#include "AssertHelper.h"
using namespace urchin;

void NavMeshGeneratorTest::holeOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(0.0, 1.0, 0.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFace[2]> - <hole>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 8); //8 points for a square with a square hole inside
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 8); //8 triangles for a square with a square hole inside
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeOnWalkableFaceEdge()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(-1.0, 1.0, -1.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 6);
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 4);
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeOverlapOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(-2.0, 1.0, -2.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 6);
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[0], Point3<float>(2.0, 0.01, 2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[1], Point3<float>(2.0, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[2], Point3<float>(-0.8, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[3], Point3<float>(-0.8, 0.01, -0.8));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[4], Point3<float>(-2.0, 0.01, -0.8));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[5], Point3<float>(-2.0, 0.01, 2.0));
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 4);
    AssertHelper::assert3Sizes(navMesh->getPolygons()[0]->getTriangles()[0]->getIndices(), new std::size_t[3]{3, 1, 2});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[0]->getTriangles()[1]->getIndices(), new std::size_t[3]{5, 1, 3});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[0]->getTriangles()[2]->getIndices(), new std::size_t[3]{0, 1, 5});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[0]->getTriangles()[3]->getIndices(), new std::size_t[3]{4, 5, 3});
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeAndCrossingHoleOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(0.5, 0.01, 0.5)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(-0.5, 1.0, -0.5)), true, holeShape);
    auto crossingHoleShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(0.5, 0.01, 3.0)).get());
    auto crossingHoleObject = std::make_shared<AIObject>("crossingHole", Transform<float>(Point3<float>(1.0, 1.0, 0.0)), true, crossingHoleShape);

    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    aiWorld.addEntity(crossingHoleObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 4);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<hole[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<[walkableFace[2]] - [crossingHole]{0}> - <hole>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 8);
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 8);
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName()=="<[walkableFace[2]] - [crossingHole]{1}>");
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[0], Point3<float>(2.0, 0.01, 2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[1], Point3<float>(2.0, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[2], Point3<float>(1.7, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[3], Point3<float>(1.7, 0.01, 2.0));
    AssertHelper::assertTrue(navMesh->getPolygons()[3]->getName()=="<crossingHole[2]>");
}

void NavMeshGeneratorTest::moveHoleOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObjectLeft = std::make_shared<AIObject>("walkableFaceLeft", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), false, walkableShape);
    auto walkableFaceObjectRight = std::make_shared<AIObject>("walkableFaceRight", Transform<float>(Point3<float>(5.0, 0.0, 0.0)), false, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(0.0, 1.0, 0.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObjectLeft);
    aiWorld.addEntity(walkableFaceObjectRight);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 3);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFaceRight[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<walkableFaceLeft[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName()=="<hole[2]>");

    holeObject->updateTransform(Point3<float>(5.0, 1.0, 0.0), Quaternion<float>());

    navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 3);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFaceLeft[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<walkableFaceRight[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName()=="<hole[2]>");
}

void NavMeshGeneratorTest::removeHoleFromWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)).get());
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(0.0, 1.0, 0.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFace[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");

    aiWorld.removeEntity(holeObject);

    navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 1);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFace[2]>");
}

void NavMeshGeneratorTest::linksRecreatedAfterMove()
{
    auto cubeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(0.5, 0.5, 0.5)).get());
    auto cube1Moving = std::make_shared<AIObject>("cube1", Transform<float>(Point3<float>(0.5, 1.5, 0.0)), false, cubeShape);
    auto cube2AffectedByMove = std::make_shared<AIObject>("cube2", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), false, cubeShape);
    auto cube3WitLinkToCube2 = std::make_shared<AIObject>("cube3", Transform<float>(Point3<float>(-2.0, 0.0, 0.0)), false, cubeShape);

    AIWorld aiWorld;
    aiWorld.addEntity(cube1Moving);
    aiWorld.addEntity(cube2AffectedByMove);
    aiWorld.addEntity(cube3WitLinkToCube2);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);
    auto cube1MovingPolygon = navMesh->getPolygons()[0];
    auto cube2AffectedByMovePolygon = navMesh->getPolygons()[1];
    auto cube3WitLinkToCube1Polygon = navMesh->getPolygons()[2];

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 3);
    AssertHelper::assertString(cube1MovingPolygon->getName(), "<cube1[2]>");
    AssertHelper::assertString(cube2AffectedByMovePolygon->getName(), "<cube2[2]>");
    AssertHelper::assertString(cube3WitLinkToCube1Polygon->getName(), "<cube3[2]>");
    AssertHelper::assertUnsignedInt(countPolygonLinks(cube3WitLinkToCube1Polygon, cube2AffectedByMovePolygon), 1);

    cube1Moving->updateTransform(Point3<float>(1.0, 1.5, 0.0), Quaternion<float>());

    navMesh = navMeshGenerator.generate(aiWorld);
    auto newCube1MovingPolygon = navMesh->getPolygons()[2];
    auto newCube2AffectedByMovePolygon = navMesh->getPolygons()[1];
    auto newCube3WitLinkToCube1Polygon = navMesh->getPolygons()[0];

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 3);
    AssertHelper::assertString(newCube1MovingPolygon->getName(), "<cube1[2]>");
    AssertHelper::assertString(newCube2AffectedByMovePolygon->getName(), "<cube2[2]>");
    AssertHelper::assertString(newCube3WitLinkToCube1Polygon->getName(), "<cube3[2]>");
    AssertHelper::assertUnsignedInt(countPolygonLinks(newCube3WitLinkToCube1Polygon, newCube2AffectedByMovePolygon), 1);
    AssertHelper::assertUnsignedInt(countPolygonLinks(newCube3WitLinkToCube1Polygon, cube2AffectedByMovePolygon), 0);
}

unsigned int NavMeshGeneratorTest::countPolygonLinks(const std::shared_ptr<NavPolygon> &sourcePolygon, const std::shared_ptr<NavPolygon> &targetPolygon)
{
    unsigned int countLinks = 0;
    for(const auto &triangle : sourcePolygon->getTriangles())
    {
        for(const auto &link : triangle->getLinks())
        {
            if(link->getTargetTriangle()->getNavPolygon() == targetPolygon)
            {
                countLinks++;
            }
        }
    }
    return countLinks;
}

std::shared_ptr<NavMeshAgent> NavMeshGeneratorTest::buildNavMeshAgent()
{
    NavMeshAgent navMeshAgent(2.0, 0.2);
    navMeshAgent.setJumpDistance(1.5);
    return std::make_shared<NavMeshAgent>(navMeshAgent);
}

CppUnit::Test *NavMeshGeneratorTest::suite()
{
    auto *suite = new CppUnit::TestSuite("NavMeshGeneratorTest");

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFace", &NavMeshGeneratorTest::holeOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFaceEdge", &NavMeshGeneratorTest::holeOnWalkableFaceEdge));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOverlapOnWalkableFace", &NavMeshGeneratorTest::holeOverlapOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeAndCrossingHoleOnWalkableFace", &NavMeshGeneratorTest::holeAndCrossingHoleOnWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("moveHoleOnWalkableFace", &NavMeshGeneratorTest::moveHoleOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("removeHoleFromWalkableFace", &NavMeshGeneratorTest::removeHoleFromWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("linksRecreatedAfterMove", &NavMeshGeneratorTest::linksRecreatedAfterMove));

    return suite;
}
