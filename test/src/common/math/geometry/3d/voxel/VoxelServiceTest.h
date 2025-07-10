#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class VoxelServiceTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void noVoxelToAABBoxes();
        void oneVoxelToAABBoxes();
};
