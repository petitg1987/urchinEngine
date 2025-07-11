#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class Triangle3DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void closestPoint();
        void projectedPointInsideTriangle();

        void parallelLineIntersection();
        void onlyLineIntersection();
        void noRayIntersection();
        void rayIntersection();

};
