#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ResizePolygon2DServiceTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void reduceTriangle();
};
