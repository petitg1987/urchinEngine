#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class MinEnclosingSphereTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void twoPoints();
        void frustumPoints();
        void rotatedFrustumPoints();
};
