#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FileUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void getDirectoryUnix();
        void getDirectoryWindows();

        void getFilesRecursiveWithSpecialChar();
};
