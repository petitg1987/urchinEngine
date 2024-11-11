#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SoundFileReaderTest final : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void cursorAfterRead();
        void cursorAfterMove();
};
