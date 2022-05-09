#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class OBBoxTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void noIntersection();
        void leftToRightLineIntersection();
        void rightToLeftLineIntersection();
        void farToNearLineIntersection();
        void nearToFarLineIntersection();
        void bottomRightToTopLeftLineIntersection();
        void oBBoxObliqueIntersection();
};
