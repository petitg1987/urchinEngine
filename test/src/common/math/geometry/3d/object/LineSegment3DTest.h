#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class LineSegment3DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void closestPoint();

};