#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class InertiaCalculationTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void boxInertiaCalculation();
};
