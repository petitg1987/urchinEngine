#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ModelSetDisplayerTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void test();
};
