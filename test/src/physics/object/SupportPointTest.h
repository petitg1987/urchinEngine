#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SupportPointTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void sphereSupportPoint();
        void boxSupportPoint();
        void capsuleSupportPoint();
        void cylinderSupportPoint();
        void coneSupportPoint();
        void convexHullSupportPoint();
};
