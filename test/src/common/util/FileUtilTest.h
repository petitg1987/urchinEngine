#ifndef URCHINENGINE_FILEUTILTEST_H
#define URCHINENGINE_FILEUTILTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FileUtilTest : public CppUnit::TestFixture {
    public:
        static CppUnit::Test* suite();

        void simplifyDirectoryPathUnix();
        void simplifyDirectoryPathWindow();

        void relativePath();
        void relativePathEqual();
};

#endif
