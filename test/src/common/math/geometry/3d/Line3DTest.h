#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class Line3DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void orthogonalProjection();
};
