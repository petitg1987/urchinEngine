#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class Line3DTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void orthogonalProjection();
};
