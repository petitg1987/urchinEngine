#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GenericRendererComparatorTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void depthTestOrdering();
};
