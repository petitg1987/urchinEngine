#ifndef URCHINENGINE_BODYMANAGERTEST_H
#define URCHINENGINE_BODYMANAGERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class BodyManagerTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void addAndRemoveBody();
};

#endif
