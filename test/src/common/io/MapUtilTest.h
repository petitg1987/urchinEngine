#ifndef URCHINENGINE_MAPUTILTEST_H
#define URCHINENGINE_MAPUTILTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include "UrchinCommon.h"

class MapUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test *suite();

        void emptyMap();
        void simpleMap();
        void trickyMapValues();
};

#endif
