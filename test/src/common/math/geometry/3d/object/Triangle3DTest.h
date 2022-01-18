#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class Triangle3DTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void closestPoint();

};
