#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class Rectangle2DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void noOverlap();
        void touchingNoOverlap();
        void partialOverlap();
        void fullOverlap();
};
