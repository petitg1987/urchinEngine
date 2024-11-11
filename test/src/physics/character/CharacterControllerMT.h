#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class CharacterControllerMT final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();
};
