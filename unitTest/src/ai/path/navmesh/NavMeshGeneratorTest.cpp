#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"

#include "NavMeshGeneratorTest.h"
#include "AssertHelper.h"
using namespace urchin;

void NavMeshGeneratorTest::holeOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(0.0, 1.0, 0.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshConfig(buildNavMeshConfig());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFace[2]> - <hole>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 8); //8 points for a square with a square hole inside
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 8); //8 triangles for a square with a square hole inside
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

void NavMeshGeneratorTest::holeEdgeOnWalkableFace()
{
    auto walkableShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(2.0, 0.01, 2.0)));
    auto walkableFaceObject = std::make_shared<AIObject>("walkableFace", Transform<float>(Point3<float>(0.0, 0.0, 0.0)), true, walkableShape);
    auto holeShape = std::make_shared<AIShape>(std::make_shared<BoxShape<float>>(Vector3<float>(1.0, 0.01, 1.0)));
    auto holeObject = std::make_shared<AIObject>("hole", Transform<float>(Point3<float>(-1.0, 1.0, -1.0)), true, holeShape);
    AIWorld aiWorld;
    aiWorld.addEntity(walkableFaceObject);
    aiWorld.addEntity(holeObject);
    NavMeshGenerator navMeshGenerator;
    navMeshGenerator.setNavMeshConfig(buildNavMeshConfig());

    std::shared_ptr<NavMesh> navMesh = navMeshGenerator.generate(aiWorld);

    AssertHelper::assertUnsignedInt(navMesh->getPolygons().size(), 2);
    AssertHelper::assertTrue(navMesh->getPolygons()[0]->getName()=="<walkableFace[2]> - <hole>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getPoints().size(), 8); //8 points for a square with a square hole inside
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[0]->getTriangles().size(), 8); //8 triangles for a square with a square hole inside
    AssertHelper::assertTrue(navMesh->getPolygons()[1]->getName()=="<hole[2]>");
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getPoints().size(), 4); //4 points of "hole" polygon
    AssertHelper::assertUnsignedInt(navMesh->getPolygons()[1]->getTriangles().size(), 2); //2 triangles of "hole" polygon
}

std::shared_ptr<NavMeshConfig> NavMeshGeneratorTest::buildNavMeshConfig()
{
    NavMeshAgent navMeshAgent(2.0, 0.0);
    return std::make_shared<NavMeshConfig>(navMeshAgent);
}

CppUnit::Test *NavMeshGeneratorTest::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("NavMeshGeneratorTest");

    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeOnWalkableFace", &NavMeshGeneratorTest::holeOnWalkableFace));
    suite->addTest(new CppUnit::TestCaller<NavMeshGeneratorTest>("holeEdgeOnWalkableFace", &NavMeshGeneratorTest::holeEdgeOnWalkableFace));

    return suite;
}
