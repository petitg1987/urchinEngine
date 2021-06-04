#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class CSGPolygonTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void simplifySquare();
        void simplifyMergeTwoNearPoints();
        void simplifyWithThreeNearPoints();
        void simplifyMergeStartEndPoints();
        void simplifyFlatTriangle1();
        void simplifyFlatTriangle2();
        void simplifyUselessPoint();
        void simplifyTwoUselessPoints();
        void simplifyCorridor();
        void simplifyCorridorWithClosePoints();
        void simplifyFlatAngleAndNearPoints();
};
