#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ResizeConvexHull3DTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void reduceBox();

        void nullExpandTetrahedron();
        void expandTetrahedron();
};
