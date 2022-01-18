#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ConvexHull3DTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void reduceBox();

        void nullExpandTetrahedron();
        void expandTetrahedron();
};
