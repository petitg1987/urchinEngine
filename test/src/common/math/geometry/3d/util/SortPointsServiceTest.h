#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SortPointsServiceTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void threePointsClockwise();
        void threePointsCounterClockwise();

        void fourPointsClockwise();
        void fourPointsCounterClockwiseZNormal();
        void fourPointsCounterClockwiseYNormal();
};
