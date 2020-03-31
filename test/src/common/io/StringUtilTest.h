#ifndef URCHINENGINE_STRINGUTILTEST_H
#define URCHINENGINE_STRINGUTILTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include "UrchinCommon.h"

class StringUtilTest : public CppUnit::TestFixture
{
    public:
        static CppUnit::Test *suite();

        void splitString();
        void splitEmptyString();
};

#endif
