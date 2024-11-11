#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class MapSerializerTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void emptyMap();
        void simpleMap();
        void trickyMapValues();
};
