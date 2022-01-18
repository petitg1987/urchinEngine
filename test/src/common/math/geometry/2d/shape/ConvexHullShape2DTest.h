#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ConvexHullShape2DTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void convexHullBox();
        void identicalPoints();
};
