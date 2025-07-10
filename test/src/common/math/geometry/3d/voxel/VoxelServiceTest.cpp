#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/math/geometry/3d/voxel/VoxelServiceTest.h>
#include <AssertHelper.h>
using namespace urchin;

void VoxelServiceTest::noVoxelToAABBoxes() {
    VoxelGrid voxelGrid(0.5f, Point3(1.0f, 1.0f, 1.0f));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 0);
}

void VoxelServiceTest::oneVoxelToAABBoxes() {
    VoxelGrid voxelGrid(0.5f, Point3(1.0f, 1.0f, 1.0f));
    voxelGrid.addVoxel(Point3(0, 0, 0));

    std::vector<AABBox<float>> boxes = VoxelService().voxelGridToAABBoxes(voxelGrid);

    AssertHelper::assertUnsignedIntEquals(boxes.size(), 1);
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMin(), Point3(0.75f, 0.75f, 0.75f));
    AssertHelper::assertPoint3FloatEquals(boxes[0].getMax(), Point3(1.25f, 1.25f, 1.25f));
}

CppUnit::Test* VoxelServiceTest::suite() {
    auto* suite = new CppUnit::TestSuite("VoxelServiceTest");

    suite->addTest(new CppUnit::TestCaller("noVoxelToAABBoxes", &VoxelServiceTest::noVoxelToAABBoxes));
    suite->addTest(new CppUnit::TestCaller("oneVoxelToAABBoxes", &VoxelServiceTest::oneVoxelToAABBoxes));

    return suite;
}

