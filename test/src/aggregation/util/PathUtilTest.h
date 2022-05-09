#pragma once

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PathUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void relativePath();
        void relativePathEqual();
        void relativePathOnDifferentDisk();

        void simplifyDirectoryPathUnix();
        void simplifyDirectoryPathWindows();
};
