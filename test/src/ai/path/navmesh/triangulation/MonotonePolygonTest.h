#ifndef URCHINENGINE_MONOTONEPOLYGONTEST_H
#define URCHINENGINE_MONOTONEPOLYGONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class MonotonePolygonTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void singleTriangle();

        void oneSplitVertex();
        void twoSplitVertex();
        void oneMergeVertex();
        void twoRegularVertex();
        void splitAndMergeVertex();

        void polygonOneHole1();
        void polygonOneHole2();
        void polygonTwoHoles1();
        void polygonTwoHoles2();
        void polygonTwoHoles3();
};

#endif
