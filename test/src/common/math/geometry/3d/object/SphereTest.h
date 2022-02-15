#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class SphereTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void sphereLineIntersection();

        void sphereAABBoxSideCollision();
        void sphereAABBoxNoSideCollision();
        void sphereAABBoxCornerCollision();
        void sphereAABBoxNoCornerCollision();
};
