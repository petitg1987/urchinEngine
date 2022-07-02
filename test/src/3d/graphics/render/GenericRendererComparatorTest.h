#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GenericRendererComparatorTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void depthTestOrdering();
};
