#ifndef URCHINENGINE_FUNNELALGORITHMTEST_H
#define URCHINENGINE_FUNNELALGORITHMTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FunnelAlgorithmTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void straightPath();
        void cornerPath();
};

#endif
