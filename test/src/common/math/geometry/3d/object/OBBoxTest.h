#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class OBBoxTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void noIntersection();
        void leftRightLineIntersection();
        void rightLeftLineIntersection();
        void bottomLeftLineIntersection();
        void oBBoxObliqueIntersection();
};