#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class TerrainObstacleServiceTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void oneSquare();
        void twoAlignedSquares();
        void twoSquaresSamePoint();
        void squaresInUForm();
};
