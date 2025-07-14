#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class MeshSimplificationServiceTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void simplifyOneEdge();
        void simplifyTwoConsecutiveEdges();
};
