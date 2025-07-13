#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/voxel/VoxelServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void VoxelServiceTest::voxelizeObject() {
    std::vector<Point3<float>> vertices;
    vertices.emplace_back(1.0f, 1.0f, -1.0f);
    vertices.emplace_back(3.0f, 1.0f, -1.0f);
    vertices.emplace_back(2.0f, 3.1f, 0.0f);
    vertices.emplace_back(2.0f, 1.0f, 1.0f);

    std::vector<std::array<uint32_t, 3>> triangleIndices;
    triangleIndices.insert(triangleIndices.end(), {0, 1, 2}); //back
    triangleIndices.insert(triangleIndices.end(), {0, 2, 3}); //left
    triangleIndices.insert(triangleIndices.end(), {1, 2, 3}); //right
    triangleIndices.insert(triangleIndices.end(), {0, 1, 3}); //bottom

    VoxelGrid voxelGrid = VoxelService().voxelizeManifoldMesh(1.0, MeshData(vertices, triangleIndices));

    AssertHelper::assertUnsignedIntEquals(voxelGrid.getVoxels().size(), 6);
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(1, 1, 0)));
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(1, 0, 0)));
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(0, 1, 0)));
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(0, 0, 1)));
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(1, 0, 1)));
    AssertHelper::assertTrue(voxelGrid.getVoxels().contains(Point3(0, 0, 0)));
}

void VoxelServiceTest::noVoxelToAABBoxes() {
    VoxelGrid voxelGrid(Vector3(0.5f, 0.5f, 0.5f), Point3(1.0f, 1.0f, 1.0f));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 0);
}

void VoxelServiceTest::oneVoxelToAABBoxes() {
    VoxelGrid voxelGrid(Vector3(0.5f, 0.5f, 0.5f), Point3(1.0f, 1.0f, 1.0f));
    voxelGrid.addVoxel(Point3(0, 0, 0));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 1);
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMin(), Point3(0.75f, 0.75f, 0.75f));
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMax(), Point3(1.25f, 1.25f, 1.25f));
}

void VoxelServiceTest::threeVoxelsToAABBoxes() {
    VoxelGrid voxelGrid(Vector3(0.5f, 0.5f, 0.5f), Point3(1.0f, 1.0f, 1.0f));
    voxelGrid.addVoxel(Point3(0, 0, 0));
    voxelGrid.addVoxel(Point3(0, 1, 0));
    voxelGrid.addVoxel(Point3(0, 2, 0));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 1);
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMin(), Point3(0.75f, 0.75f, 0.75f));
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMax(), Point3(1.25f, 2.25f, 1.25f));
}

void VoxelServiceTest::twoDistinctVoxelsToAABBoxes() {
    VoxelGrid voxelGrid(Vector3(0.5f, 0.5f, 0.5f), Point3(1.0f, 1.0f, 1.0f));
    voxelGrid.addVoxel(Point3(0, 0, 0));
    voxelGrid.addVoxel(Point3(1, 0, 0));
    voxelGrid.addVoxel(Point3(3, 0, 0));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 2);
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMin(), Point3(2.25f, 0.75f, 0.75f));
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMax(), Point3(2.75f, 1.25f, 1.25f));
    AssertHelper::assertPoint3FloatEquals(boxes[1].getMin(), Point3(0.75f, 0.75f, 0.75f));
    AssertHelper::assertPoint3FloatEquals(boxes[1].getMax(), Point3(1.75f, 1.25f, 1.25f));
}

void VoxelServiceTest::voxelsToAABBoxes() {
    VoxelGrid voxelGrid(Vector3(1.0f, 1.0f, 1.0f), Point3(0.5f, 0.5f, 0.5f));
    voxelGrid.addVoxel(Point3(0, 0, 0));
    voxelGrid.addVoxel(Point3(1, 0, 0));
    voxelGrid.addVoxel(Point3(0, 0, 1));
    voxelGrid.addVoxel(Point3(1, 0, 1));
    voxelGrid.addVoxel(Point3(1, 1, 0));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 3);
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMin(), Point3(1.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMax(), Point3(2.0f, 2.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(boxes[1].getMin(), Point3(0.0f, 0.0f, 1.0f));
    AssertHelper::assertPoint3FloatEquals(boxes[1].getMax(), Point3(2.0f, 1.0f, 2.0f));
    AssertHelper::assertPoint3FloatEquals(boxes[2].getMin(), Point3(0.0f, 0.0f, 0.0f));
    AssertHelper::assertPoint3FloatEquals(boxes[2].getMax(), Point3(1.0f, 1.0f, 1.0f));
}

CppUnit::Test* VoxelServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("VoxelServiceTest");

    suite->addTest(new CppUnit::TestCaller("voxelizeObject", &VoxelServiceTest::voxelizeObject));

    suite->addTest(new CppUnit::TestCaller("noVoxelToAABBoxes", &VoxelServiceTest::noVoxelToAABBoxes));
    suite->addTest(new CppUnit::TestCaller("oneVoxelToAABBoxes", &VoxelServiceTest::oneVoxelToAABBoxes));
    suite->addTest(new CppUnit::TestCaller("threeVoxelsToAABBoxes", &VoxelServiceTest::threeVoxelsToAABBoxes));
    suite->addTest(new CppUnit::TestCaller("twoDistinctVoxelsToAABBoxes", &VoxelServiceTest::twoDistinctVoxelsToAABBoxes));
    suite->addTest(new CppUnit::TestCaller("voxelsToAABBoxes", &VoxelServiceTest::voxelsToAABBoxes));

    return suite;
}

