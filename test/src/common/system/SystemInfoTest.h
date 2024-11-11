#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SystemInfoTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void retrieveOsInfo();
        void retrieveCpuCores();
        void retrieveTotalMemory();
        void retrieveGraphicsCardNames();
};
