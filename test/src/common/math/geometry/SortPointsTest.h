#ifndef URCHINENGINE_SORTPOINTSTEST_H
#define URCHINENGINE_SORTPOINTSTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SortPointsTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test *suite();

        void threePointsClockwise();
        void threePointsCounterClockwise();

        void fourPointsClockwise();
        void fourPointsCounterClockwiseZNormal();
        void fourPointsCounterClockwiseYNormal();
};

#endif
