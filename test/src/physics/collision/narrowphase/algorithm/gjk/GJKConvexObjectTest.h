#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKConvexObjectTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void separateSphereAndBox();
        void overlapSphereAndBox();
};
