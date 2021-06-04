#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class PointTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void triviallyCopyable();

};
