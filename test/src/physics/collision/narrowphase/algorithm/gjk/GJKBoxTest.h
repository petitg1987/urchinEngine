#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKBoxTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void identicalAABBox();
        void separateAABBox();
        void overlapAABBox();

        void identicalOBBox();
        void separateEdgeOBBox();
        void separateCornerOBBox();
        void overlapOnEdgeOBBox();

        void separateOBBoxAABBox();
        void overlapOBBoxAABBox();
};
