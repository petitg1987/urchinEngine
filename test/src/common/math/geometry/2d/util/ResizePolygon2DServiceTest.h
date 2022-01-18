#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ResizePolygon2DServiceTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void reduceTriangle();
};
