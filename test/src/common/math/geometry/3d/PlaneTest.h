#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PlaneTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void orthogonalProjection();
};
