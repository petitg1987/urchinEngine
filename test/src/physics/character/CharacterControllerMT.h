#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <memory>
#include <UrchinPhysicsEngine.h>

class CharacterControllerMT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();
};
