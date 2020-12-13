#ifndef URCHINENGINE_FRUSTUMSHADOWDATATEST_H
#define URCHINENGINE_FRUSTUMSHADOWDATATEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FrustumShadowDataTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void test();
};

#endif
