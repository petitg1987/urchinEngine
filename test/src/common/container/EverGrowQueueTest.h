#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EverGrowQueueTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void pushAndPop();
};
