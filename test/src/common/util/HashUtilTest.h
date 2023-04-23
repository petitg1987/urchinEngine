#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class HashUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void hashBool();
        void hashFloat();
};
