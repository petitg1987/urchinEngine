#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKContinuousCollisionAlgorithmTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void rayTest();
};
