#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKSphereTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void identicalSphere();
        void separateSphere();
        void overlapSphere();
};
