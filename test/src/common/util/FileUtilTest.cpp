#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "FileUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void FileUtilTest::getDirectoryFromUnixPath() {
    std::string filenamePath = "/home/jean/binaryFile";

    std::string result = FileUtil::instance()->getDirectoryFrom(filenamePath);

    AssertHelper::instance()->assertTrue(result == "/home/jean/");
}

void FileUtilTest::getDirectoryFromWindowsPath() {
    std::string filenamePath = R"(C:\home\jean\binaryFile)";

    std::string result = FileUtil::instance()->getDirectoryFrom(filenamePath);

    AssertHelper::instance()->assertTrue(result == R"(C:\home\jean\)");
}

void FileUtilTest::relativePath() {
    std::string referenceDirectory = "xxx/yyy/zzz/www/";
    std::string path = "xxx/yyy/aaa/bbb/";

    std::string result = FileUtil::instance()->getRelativePath(referenceDirectory, path);

    AssertHelper::instance()->assertTrue(result == "../../aaa/bbb/");
}

void FileUtilTest::relativePathEqual() {
    std::string referenceDirectory = "xxx/yyy/";
    std::string path = "xxx/yyy/";

    std::string result = FileUtil::instance()->getRelativePath(referenceDirectory, path);

    AssertHelper::instance()->assertTrue(result.empty());
}

void FileUtilTest::simplifyDirectoryPathUnix() {
    std::string directoryPath = "xxx/yyy/../zzz/www/../rrr/";

    std::string result = FileUtil::instance()->simplifyDirectoryPath(directoryPath);

    AssertHelper::instance()->assertTrue(result == "xxx/zzz/rrr/");
}

void FileUtilTest::simplifyDirectoryPathWindows() {
    std::string directoryPath = R"(xxx\yyy\..\zzz\www\..\rrr\)";

    std::string result = FileUtil::instance()->simplifyDirectoryPath(directoryPath);

    AssertHelper::instance()->assertTrue(result == R"(xxx\zzz\rrr\)");
}

CppUnit::Test* FileUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("FileUtilTest");

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("getDirectoryFromUnixPath", &FileUtilTest::getDirectoryFromUnixPath));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("getDirectoryFromWindowsPath", &FileUtilTest::getDirectoryFromWindowsPath));

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePath", &FileUtilTest::relativePath));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePathEqual", &FileUtilTest::relativePathEqual));

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathUnix", &FileUtilTest::simplifyDirectoryPathUnix));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathWindows", &FileUtilTest::simplifyDirectoryPathWindows));

    return suite;
}
