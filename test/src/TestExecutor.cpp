#include <cppunit/ui/text/TestRunner.h>

#include <common/collision/GJKAlgorithmTest.h>
#include <common/io/map/MapSerializerTest.h>
#include <common/io/uda/UdaParserTest.h>
#include <common/system/SystemInfoTest.h>
#include <common/util/StringUtilTest.h>
#include <common/util/HashUtilTest.h>
#include <common/util/FileUtilTest.h>
#include <common/container/EverGrowQueueTest.h>
#include <common/container/EverGrowHashMapTest.h>
#include <common/container/EverGrowHashSetTest.h>
#include <common/math/algebra/QuaternionTest.h>
#include <common/math/algebra/point/PointTest.h>
#include <common/math/algebra/vector/VectorTest.h>
#include <common/math/algorithm/MinEnclosingSphereTest.h>
#include <common/math/geometry/2d/object/LineSegment2DTest.h>
#include <common/math/geometry/2d/util/ResizePolygon2DServiceTest.h>
#include <common/math/geometry/2d/shape/ConvexHullShape2DTest.h>
#include <common/math/geometry/2d/Line2DTest.h>
#include <common/math/geometry/3d/object/AABBoxTest.h>
#include <common/math/geometry/3d/object/OBBoxTest.h>
#include <common/math/geometry/3d/object/SphereTest.h>
#include <common/math/geometry/3d/object/LineSegment3DTest.h>
#include <common/math/geometry/3d/object/Triangle3DTest.h>
#include <common/math/geometry/3d/object/TetrahedronTest.h>
#include <common/math/geometry/3d/object/ConvexHull3DTest.h>
#include <common/math/geometry/3d/util/SortPointsServiceTest.h>
#include <common/math/geometry/3d/voxel/VoxelServiceTest.h>
#include <common/math/geometry/3d/Line3DTest.h>
#include <common/math/geometry/3d/PlaneTest.h>
#include <common/partitioning/GridContainerTest.h>
#include <3d/graphics/render/GenericRendererComparatorTest.h>
#include <3d/scene/renderer3d/Renderer3dTest.h>
#include <3d/scene/renderer3d/model/culler/ModelOcclusionCullerTest.h>
#include <3d/scene/renderer3d/lighting/shadow/light/LightSplitShadowMapTest.h>
#include <3d/scene/ui/UIRendererTest.h>
#include <3d/scene/ui/widget/text/TextTest.h>
#include <3d/scene/ui/widget/textbox/TextBoxTest.h>
#include <3d/scene/ui/widget/textarea/TextareaTest.h>
#include <physics/shape/ShapeToAABBoxTest.h>
#include <physics/shape/ShapeToConvexObjectTest.h>
#include <physics/object/SupportPointTest.h>
#include <physics/body/BodyContainerTest.h>
#include <physics/body/InertiaCalculationTest.h>
#include <physics/collision/broadphase/aabbtree/BodyAABBTreeTest.h>
#include <physics/collision/narrowphase/algorithm/epa/EPAAlgorithmTest.h>
#include <physics/collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithmTest.h>
#include <physics/collision/bodystate/IslandContainerTest.h>
#include <physics/collision/CollisionWorldIT.h>
#include <physics/character/CharacterControllerIT.h>
#include <physics/character/CharacterControllerMT.h>
#include <ai/path/navmesh/csg/CSGPolygonTest.h>
#include <ai/path/navmesh/csg/PolygonsUnionTest.h>
#include <ai/path/navmesh/csg/PolygonsSubtractionTest.h>
#include <ai/path/navmesh/triangulation/TriangulationTest.h>
#include <ai/path/navmesh/polytope/services/TerrainObstacleServiceTest.h>
#include <ai/path/navmesh/jump/EdgeLinkDetectionTest.h>
#include <ai/path/navmesh/NavMeshGeneratorTest.h>
#include <ai/path/pathfinding/FunnelAlgorithmTest.h>
#include <ai/path/pathfinding/PathfindingAStarTest.h>
#include <sound/player/filereader/SoundFileReaderTest.h>
using namespace urchin;

void addCommonUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //collision
    runner.addTest(GJKAlgorithmTest::suite());

    //io
    runner.addTest(MapSerializerTest::suite());
    runner.addTest(UdaParserTest::suite());

    //system
    runner.addTest(SystemInfoTest::suite());

    //container
    runner.addTest(EverGrowQueueTest::suite());
    runner.addTest(EverGrowHashMapTest::suite());
    runner.addTest(EverGrowHashSetTest::suite());

    //util
    runner.addTest(FileUtilTest::suite());
    runner.addTest(HashUtilTest::suite());
    runner.addTest(StringUtilTest::suite());

    //math - algebra
    runner.addTest(QuaternionTest::suite());
    runner.addTest(PointTest::suite());
    runner.addTest(VectorTest::suite());

    //math - algorithm
    runner.addTest(MinEnclosingSphereTest::suite());

    //math - geometry
    runner.addTest(ResizePolygon2DServiceTest::suite());
    runner.addTest(LineSegment2DTest::suite());
    runner.addTest(ConvexHullShape2DTest::suite());
    runner.addTest(Line2DTest::suite());
    runner.addTest(AABBoxTest::suite());
    runner.addTest(OBBoxTest::suite());
    runner.addTest(SphereTest::suite());
    runner.addTest(LineSegment3DTest::suite());
    runner.addTest(Triangle3DTest::suite());
    runner.addTest(TetrahedronTest::suite());
    runner.addTest(ConvexHull3DTest::suite());
    runner.addTest(SortPointsServiceTest::suite());
    runner.addTest(VoxelServiceTest::suite());
    runner.addTest(Line3DTest::suite());
    runner.addTest(PlaneTest::suite());

    //partitioning
    runner.addTest(GridContainerTest::suite());
}

void add3dUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //api
    runner.addTest(GenericRendererComparatorTest::suite());

    //renderer 3d
    runner.addTest(Renderer3dTest::suite());

    //model
    runner.addTest(ModelOcclusionCullerTest::suite());

    //shadow
    runner.addTest(LightSplitShadowMapTest::suite());

    //ui
    runner.addTest(UIRendererTest::suite());
    runner.addTest(TextTest::suite());
    runner.addTest(TextBoxTest::suite());
    runner.addTest(TextareaTest::suite());
}

void addPhysicsUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //shape
    runner.addTest(ShapeToAABBoxTest::suite());
    runner.addTest(ShapeToConvexObjectTest::suite());

    //object
    runner.addTest(SupportPointTest::suite());

    //body
    runner.addTest(BodyContainerTest::suite());
    runner.addTest(InertiaCalculationTest::suite());

    //broad phase
    runner.addTest(BodyAABBTreeTest::suite());

    //narrow phase
    runner.addTest(EPAAlgorithmTest::suite());
    runner.addTest(GJKContinuousCollisionAlgorithmTest::suite());

    //island
    runner.addTest(IslandContainerTest::suite());
}

void addPhysicsIntegrationTests(CppUnit::TextUi::TestRunner& runner) {
    //collision world
    runner.addTest(CollisionWorldIT::suite());

    //character
    runner.addTest(CharacterControllerIT::suite());
}

void addPhysicsMonkeyTests(CppUnit::TextUi::TestRunner& runner) {
    //character
    runner.addTest(CharacterControllerMT::suite());
}

void addAiUnitTests(CppUnit::TextUi::TestRunner& runner) {
    //navigation mesh
    runner.addTest(CSGPolygonTest::suite());
    runner.addTest(PolygonsUnionTest::suite());
    runner.addTest(PolygonsSubtractionTest::suite());
    runner.addTest(TriangulationTest::suite());
    runner.addTest(TerrainObstacleServiceTest::suite());
    runner.addTest(EdgeLinkDetectionTest::suite());
    runner.addTest(NavMeshGeneratorTest::suite());

    //pathfinding
    runner.addTest(FunnelAlgorithmTest::suite());
    runner.addTest(PathfindingAStarTest::suite());
}

void addSoundTests(CppUnit::TextUi::TestRunner& runner) {
    runner.addTest(SoundFileReaderTest::suite());
}

void addAllUnitTests(CppUnit::TextUi::TestRunner& runner) {
    addCommonUnitTests(runner);
    add3dUnitTests(runner);
    addPhysicsUnitTests(runner);
    addAiUnitTests(runner);
    addSoundTests(runner);
}

void addAllIntegrationTests(CppUnit::TextUi::TestRunner& runner) {
    addPhysicsIntegrationTests(runner);
}

void addAllMonkeyTests(CppUnit::TextUi::TestRunner& runner) {
    addPhysicsMonkeyTests(runner);
}

int main(int argc, char *argv[]) {
    FileSystem::instance().setupResourcesDirectory(SystemInfo::executableDirectory() + "resources/");
    ConfigService::instance().loadProperties("engine.properties");
    CppUnit::TextUi::TestRunner runner;

    std::string hasUnitTests = "no";
    std::string hasIntegrationTests = "no";
    std::string hasMonkeyTests = "no";

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "unit") == 0) {
            addAllUnitTests(runner);
            hasUnitTests = "yes";
        } else if (std::strcmp(argv[i], "integration") == 0) {
            addAllIntegrationTests(runner);
            hasIntegrationTests = "yes";
        } else if (std::strcmp(argv[i], "monkey") == 0) {
            addAllMonkeyTests(runner);
            hasMonkeyTests = "yes";
        } else {
            throw std::invalid_argument("Unknown program argument: " + std::string(argv[i]));
        }
    }

    std::cout << "Start running tests (unit: "<< hasUnitTests << ", integration: " << hasIntegrationTests << ", monkey: " << hasMonkeyTests << ")" << std::endl;
    bool success = runner.run();

    Logger::instance().purge();
    return success ? 0 : 1;
}
