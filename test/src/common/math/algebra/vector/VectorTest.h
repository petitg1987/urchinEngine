#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class VectorTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void triviallyCopyable();

};
