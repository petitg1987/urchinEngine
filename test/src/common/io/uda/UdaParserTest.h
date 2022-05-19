#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include <UrchinCommon.h>

class UdaParserTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void removeChunk();
};
