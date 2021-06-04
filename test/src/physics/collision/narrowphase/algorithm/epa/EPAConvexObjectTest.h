#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPAConvexObjectTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void overlapSphereAndBox();
        void overlapCapsuleAndTriangle();
        void overlapTriangleAndCapsule();
};
