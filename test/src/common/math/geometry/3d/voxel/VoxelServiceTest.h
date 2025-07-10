#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class VoxelServiceTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void voxelizeObject();

        void noVoxelToAABBoxes();
        void oneVoxelToAABBoxes();
        void threeVoxelsToAABBoxes();
        void twoDistinctVoxelsToAABBoxes();
        void voxelsToAABBoxes();
};
