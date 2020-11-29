#ifndef URCHINENGINE_FALLINGOBJECTIT_H
#define URCHINENGINE_FALLINGOBJECTIT_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FallingObjectIT : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void fallOnPlane();
        void fallForever();
};

#endif
