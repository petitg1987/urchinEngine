#ifndef URCHINENGINE_FILEUTILTEST_H
#define URCHINENGINE_FILEUTILTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FileUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void getDirectoryUnix();
        void getDirectoryWindows();

        void relativePath();
        void relativePathEqual();

        void simplifyDirectoryPathUnix();
        void simplifyDirectoryPathWindows();

};

#endif
