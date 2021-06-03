#ifndef URCHINENGINE_MAPSERIALIZERTEST_H
#define URCHINENGINE_MAPSERIALIZERTEST_H

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

#endif
