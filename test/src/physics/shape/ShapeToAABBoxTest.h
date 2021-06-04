#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ShapeToAABBoxTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void boxConversion();
        void coneConversion();
        void convexHullConversion();
};
