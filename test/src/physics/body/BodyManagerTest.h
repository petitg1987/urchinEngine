#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyManagerTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void addAndRemoveBody();
};
