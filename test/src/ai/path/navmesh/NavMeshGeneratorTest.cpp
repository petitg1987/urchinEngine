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
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 2); //2 triangles of "hole" polygon
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<walkableFace[2]> - <hole>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 8); //8 points for a square with a square hole inside
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 8); //8 triangles for a square with a square hole inside
}

void NavMeshGeneratorTest::holeEdgeOnWalkableFace()
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
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 2); //2 triangles of "hole" polygon
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 6);
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 4);
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
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 2); //2 triangles of "hole" polygon
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<[walkableFace[2]] - [hole]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 6);
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[0], Point3<float>(2.0, 0.01, 2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[1], Point3<float>(2.0, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[2], Point3<float>(-1.0, 0.01, -2.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[3], Point3<float>(-1.0, 0.01, -1.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[4], Point3<float>(-2.0, 0.01, -1.0));
    AssertHelper::assertPoint3FloatEquals(navMesh->getPolygons()[1]->getPoints()[5], Point3<float>(-2.0, 0.01, 2.0));
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 4);
    AssertHelper::assert3Sizes(navMesh->getPolygons()[1]->getTriangles()[0]->getIndices(), new std::size_t[3]{3, 1, 2});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[1]->getTriangles()[1]->getIndices(), new std::size_t[3]{5, 1, 3});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[1]->getTriangles()[2]->getIndices(), new std::size_t[3]{0, 1, 5});
    AssertHelper::assert3Sizes(navMesh->getPolygons()[1]->getTriangles()[3]->getIndices(), new std::size_t[3]{4, 5, 3});
}

void NavMeshGeneratorTest::moveHoleOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)).get());
    auto walkableFaceObjectLeft = std::make_shared<AIObject>("walkableFaceLeft", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto walkableFaceObjectRight = std::make_shared<AIObject>("walkableFaceRight", Transform<float>(Point3<float>(5.0, 0.0, 0.0)), true, walkableShape);
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

//TODO add tests: remove 'hole' and 'walkableObject' shouldn't have hole anymore

std::shared_ptr<NavMeshAgent> NavMeshGeneratorTest::buildNavMeshAgent()
{
    NavMeshAgent navMeshAgent(2.0, 0.0);
    return std::make_shared<NavMeshAgent>(navMeshAgent);
}

CppUnit::Test *NavMeshGeneratorTest::suite()
{
    auto *suite = new CppUnit::TestSuite("NavMeshGeneratorTest");

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFace", &NavMeshGeneratorTest::holeOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeEdgeOnWalkableFace", &NavMeshGeneratorTest::holeEdgeOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOverlapOnWalkableFace", &NavMeshGeneratorTest::holeOverlapOnWalkableFace));

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("moveHoleOnWalkableFace", &NavMeshGeneratorTest::moveHoleOnWalkableFace));

    return suite;
}
