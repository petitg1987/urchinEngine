#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class Triangle3DTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void closestPoint();
        void projectedPointInsideTriangle();

        void noRayIntersectionParallel();
        void noRayIntersectionPerpendicular();
        void noRayIntersectionOblique();
        void intersectionBeforeRay();
        void intersectionAfterRay();
        void rayIntersection();

};
