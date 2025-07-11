#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class LineSegment2DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void closestPoint();

        void noIntersection();
        void parallelLinesIntersection();
        void intersection();

        void intersectionOnOnePoint();
        void intersectionOnOnePointInt();
        void collinearIntersectionOnOnePoint();

        void collinearLinesNoIntersectionSameDirection();
        void collinearLinesNoIntersectionOppositeDirection();

        void collinearLinesIntersectionSameDirection();
        void collinearLinesIntersectionSameDirection2();
        void collinearLinesIntersectionOppositeDirection();
        void collinearLinesIntersectionOppositeDirection2();
        void collinearLinesFullIntersectionSameDirection();
        void collinearLinesFullIntersectionOppositeDirection();
};
