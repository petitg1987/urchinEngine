#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ShapeToConvexObjectTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void boxConversion();
        void capsuleConversion();
        void cylinderConversion();
        void convexHullConversion();
        void sphereConversion();
};
