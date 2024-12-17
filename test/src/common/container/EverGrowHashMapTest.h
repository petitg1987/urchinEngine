#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EverGrowHashMapTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void insertAndGet();
};
