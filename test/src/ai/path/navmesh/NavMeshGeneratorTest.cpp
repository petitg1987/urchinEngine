#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include <UrchinCommon.h>

#include <ai/path/navmesh/NavMeshGeneratorTest.h>
#include <AssertHelper.h>
using namespace urchin;

void NavMeshGeneratorTest::holeOnWalkableFace() {
    auto walkableShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShape));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.0f, 0.01f, 1.0f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(0.0f, 1.0f, 0.0f)), true, std::move(holeShape));
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<walkableFace[2]> - <hole>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getPoints().size(), 8); //8 points for a square with a square hole inside
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getTriangles().size(), 8); //8 triangles for a square with a square hole inside
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<hole[2]>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeOnWalkableFaceEdge() {
    auto walkableShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShape));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.0f, 0.01f, 1.0f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(-1.0f, 1.0f, -1.0f)), true, std::move(holeShape));
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getPoints().size(), 6);
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getTriangles().size(), 4);
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<hole[2]>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeOverlapOnWalkableFace() {
    auto walkableShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShape));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.0f, 0.01f, 1.0f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(-2.0f, 1.0f, -2.0f)), true, std::move(holeShape));
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getPoints().size(), 6);
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[0], Point3(-2.0f, 0.01f, 2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[1], Point3(-2.0f, 0.01f, -0.8f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[2], Point3(-0.8f, 0.01f, -0.8f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[3], Point3(-0.8f, 0.01f, -2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[4], Point3(2.0f, 0.01f, -2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[0]->getPoints()[5], Point3(2.0f, 0.01f, 2.0f));
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[0]->getTriangles().size(), 4);
    AssertHelper::assert3SizesEquals(navMesh->getPolygons()[0]->getTriangles()[0]->getIndices(), {1, 0, 5});
    AssertHelper::assert3SizesEquals(navMesh->getPolygons()[0]->getTriangles()[1]->getIndices(), {5, 4, 3});
    AssertHelper::assert3SizesEquals(navMesh->getPolygons()[0]->getTriangles()[2]->getIndices(), {2, 1, 5});
    AssertHelper::assert3SizesEquals(navMesh->getPolygons()[0]->getTriangles()[3]->getIndices(), {5, 3, 2});
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<hole[2]>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeAndCrossingHoleOnWalkableFace() {
    auto walkableShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShape));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.01f, 0.5f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(-0.5f, 1.0f, -0.5f)), true, std::move(holeShape));
    auto crossingHoleShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.01f, 3.0f)));
    auto crossingHoleObject = std::make_shared<AIObject>("crossingHole", Transform<float>(Point3(1.0f, 1.0f, 0.0f)), true, std::move(crossingHoleShape));

    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    aiWorld.addEntity(crossingHoleObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 4);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<hole[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<[walkableFace[2]] - [crossingHole]{0}> - <hole>");
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getPoints().size(), 8);
    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons()[1]->getTriangles().size(), 8);
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName() == "<[walkableFace[2]] - [crossingHole]{1}>");
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[0], Point3(1.7f, 0.01f, 2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[1], Point3(1.7f, 0.01f, -2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[2], Point3(2.0f, 0.01f, -2.0f));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[2]->getPoints()[3], Point3(2.0f, 0.01f, 2.0f));
    AssertHelper::assertTrue(navMesh->getPolygons()[3]->getName() == "<crossingHole[2]>");
}

void NavMeshGeneratorTest::surroundedWalkableFace() {
    float overlap = 0.0001f;
    auto walkableShapeFront = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.5f, 0.01f, 0.5f)));
    auto walkableFaceObjectFront = std::make_shared<AIObject>("walkableFaceFront", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShapeFront));
    auto walkableShapeLeft = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.01f, 0.5f)));
    auto walkableFaceObjectLeft = std::make_shared<AIObject>("walkableFaceLeft", Transform<float>(Point3(-1.0f, 0.0f, 1.0f - overlap)), true, std::move(walkableShapeLeft));
    auto walkableShapeRight = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.01f, 0.5f)));
    auto walkableFaceObjectRight = std::make_shared<AIObject>("walkableFaceRight", Transform<float>(Point3(1.0f, 0.0f, 1.0f - overlap)), true, std::move(walkableShapeRight));
    auto walkableShapeBack = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.5f, 0.01f, 0.5f)));
    auto walkableFaceObjectBack = std::make_shared<AIObject>("walkableFaceBack", Transform<float>(Point3(0.0f, 0.0f, 2.0f - overlap * 2.0f)), true, std::move(walkableShapeBack));
    auto walkableShapeSurrounded = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.01f, 0.5f)));
    auto walkableFaceObjectSurrounded = std::make_shared<AIObject>("walkableFaceSurrounded", Transform<float>(Point3(0.0f, -0.01f, 1.0f)), true, std::move(walkableShapeSurrounded));

    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObjectFront);
    aiWorld.addEntity(walkableFaceObjectLeft);
    aiWorld.addEntity(walkableFaceObjectRight);
    aiWorld.addEntity(walkableFaceObjectBack);
    aiWorld.addEntity(walkableFaceObjectSurrounded);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    bool surroundedObjectIsWalkable = false;
    for (auto& navPolygon : navMesh->getPolygons()) {
        surroundedObjectIsWalkable = surroundedObjectIsWalkable || navPolygon->getName().find("Surrounded") != std::string::npos;
    }
    //TO DO: bug to fix
    //AssertHelper::assertTrue(surroundedObjectIsWalkable);
}

void NavMeshGeneratorTest::moveHoleOnWalkableFace() {
    auto walkableShapeLeft = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObjectLeft = std::make_shared<AIObject>("walkableFaceLeft", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), false, std::move(walkableShapeLeft));
    auto walkableShapeRight = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObjectRight = std::make_shared<AIObject>("walkableFaceRight", Transform<float>(Point3(5.0f, 0.0f, 0.0f)), false, std::move(walkableShapeRight));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.0f, 0.01f, 1.0f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(0.0f, 1.0f, 0.0f)), true, std::move(holeShape));
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObjectLeft);
    aiWorld.addEntity(walkableFaceObjectRight);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 3);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<walkableFaceRight[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<walkableFaceLeft[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName() == "<hole[2]>");

    holeObject->updateTransform(Point3(5.0f, 1.0f, 0.0f), Quaternion<float>());

    navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 3);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<walkableFaceLeft[2]>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<walkableFaceRight[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[2]->getName() == "<hole[2]>");
}

void NavMeshGeneratorTest::removeHoleFromWalkableFace() {
    auto walkableShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(2.0f, 0.01f, 2.0f)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), true, std::move(walkableShape));
    auto holeShape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(1.0f, 0.01f, 1.0f)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3(0.0f, 1.0f, 0.0f)), true, std::move(holeShape));
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshAgent(buildNavMeshAgent());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<walkableFace[2]> - <hole>");
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName() == "<hole[2]>");

    aiWorld.removeEntity(holeObject);

    navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 1);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName() == "<walkableFace[2]>");
}

void NavMeshGeneratorTest::linksRecreatedAfterMove() {
    auto cube1Shape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.5f, 0.5f)));
    auto cube1Moving = std::make_shared<AIObject>("cube1", Transform<float>(Point3(0.5f, 1.5f, 0.0f)), false, std::move(cube1Shape));
    auto cube2Shape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.5f, 0.5f)));
    auto cube2AffectedByMove = std::make_shared<AIObject>("cube2", Transform<float>(Point3(0.0f, 0.0f, 0.0f)), false, std::move(cube2Shape));
    auto cube3Shape = std::make_unique<AIShape>(std::make_unique<BoxShape<float>>(Vector3(0.5f, 0.5f, 0.5f)));
    auto cube3WitLinkToCube2 = std::make_shared<AIObject>("cube3", Transform<float>(Point3(-2.0f, 0.0f, 0.0f)), false, std::move(cube3Shape));

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

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 3);
    AssertHelper::assertStringEquals(cube1MovingPolygon->getName(), "<cube1[2]>");
    AssertHelper::assertStringEquals(cube2AffectedByMovePolygon->getName(), "<cube2[2]>");
    AssertHelper::assertStringEquals(cube3WitLinkToCube1Polygon->getName(), "<cube3[2]>");
    AssertHelper::assertUnsignedIntEquals(countPolygonLinks(*cube3WitLinkToCube1Polygon, *cube2AffectedByMovePolygon), 1);

    cube1Moving->updateTransform(Point3(1.0f, 1.5f, 0.0f), Quaternion<float>());

    navMesh = navMeshGenerator.generate(aiWorld);
    auto newCube1MovingPolygon = navMesh->getPolygons()[2];
    auto newCube2AffectedByMovePolygon = navMesh->getPolygons()[1];
    auto newCube3WitLinkToCube1Polygon = navMesh->getPolygons()[0];

    AssertHelper::assertUnsignedIntEquals(navMesh->getPolygons().size(), 3);
    AssertHelper::assertStringEquals(newCube1MovingPolygon->getName(), "<cube1[2]>");
    AssertHelper::assertStringEquals(newCube2AffectedByMovePolygon->getName(), "<cube2[2]>");
    AssertHelper::assertStringEquals(newCube3WitLinkToCube1Polygon->getName(), "<cube3[2]>");
    AssertHelper::assertUnsignedIntEquals(countPolygonLinks(*newCube3WitLinkToCube1Polygon, *newCube2AffectedByMovePolygon), 1);
    AssertHelper::assertUnsignedIntEquals(countPolygonLinks(*newCube3WitLinkToCube1Polygon, *cube2AffectedByMovePolygon), 0);
}

unsigned int NavMeshGeneratorTest::countPolygonLinks(const NavPolygon& sourcePolygon, const NavPolygon& targetPolygon) const {
    unsigned int countLinks = 0;
    for (const auto& triangle : sourcePolygon.getTriangles()) {
        for (const auto& link : triangle->getLinks()) {
            if (link->getTargetTriangle()->getNavPolygon().get() == &targetPolygon) {
                countLinks++;
            }
        }
    }
    return countLinks;
}

NavMeshAgent NavMeshGeneratorTest::buildNavMeshAgent() const {
    NavMeshAgent navMeshAgent(2.0f, 0.2f);
    navMeshAgent.setJumpDistance(1.5f);
    return navMeshAgent;
}

CppUnit::Test* NavMeshGeneratorTest::suite() {
    auto* suite = new CppUnit::TestSuite("NavMeshGeneratorTest");

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFace", &NavMeshGeneratorTest::holeOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFaceEdge", &NavMeshGeneratorTest::holeOnWalkableFaceEdge));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOverlapOnWalkableFace", &NavMeshGeneratorTest::holeOverlapOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeAndCrossingHoleOnWalkableFace", &NavMeshGeneratorTest::holeAndCrossingHoleOnWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("surroundedWalkableFace", &NavMeshGeneratorTest::surroundedWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("moveHoleOnWalkableFace", &NavMeshGeneratorTest::moveHoleOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("removeHoleFromWalkableFace", &NavMeshGeneratorTest::removeHoleFromWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("linksRecreatedAfterMove", &NavMeshGeneratorTest::linksRecreatedAfterMove));

    return suite;
}
