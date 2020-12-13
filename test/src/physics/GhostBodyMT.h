#ifndef URCHINENGINE_GHOSTBODYMT_H
#define URCHINENGINE_GHOSTBODYMT_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GhostBodyMT : public CppUnit::TestFixture { //TODO good idea to use MT ?
    public:
        static CppUnit::Test* suite();

        void test();
};

#endif
