#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FunnelAlgorithmTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void straightPath();

        void cornerPath1();
        void cornerPath2();
        void cornerPath3();
        void cornerPath4();
};
