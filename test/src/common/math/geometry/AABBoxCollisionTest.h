#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class AABBoxCollisionTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void rayRightToBox();
        void rayRightTopToBox();

        void rayInsideBox();

        void rayThroughXPlanes();
        void rayThroughYPlanes();
        void rayThroughZPlanes();
        void rayThroughXYPlanes();

        void rayInsideToXPlane();
};
