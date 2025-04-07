#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class OBBoxTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void toProjectionMatrix();

        void noIntersection();
        void leftToRightLineIntersection();
        void rightToLeftLineIntersection();
        void farToNearLineIntersection();
        void nearToFarLineIntersection();
        void bottomRightToTopLeftLineIntersection();
        void oBBoxObliqueIntersection();

        void matrixMultiplication();
};
