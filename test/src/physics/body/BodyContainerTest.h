#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyContainerTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void addAndRemoveBody();
};
