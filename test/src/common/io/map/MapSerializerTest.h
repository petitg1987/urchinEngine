#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class MapSerializerTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void emptyMap();
        void simpleMap();
        void trickyMapValues();
};
