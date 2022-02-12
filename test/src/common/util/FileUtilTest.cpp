#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include <common/util/FileUtilTest.h>
#include <AssertHelper.h>
using namespace urchin;

void FileUtilTest::getDirectoryUnix() {
    std::string filenamePath = "/home/jean/binaryFile";

    std::string_view result = FileUtil::getDirectory(filenamePath);

    AssertHelper::assertTrue(result == "/home/jean/");
}

void FileUtilTest::getDirectoryWindows() {
    std::string filenamePath = R"(C:\home\jean\binaryFile)";

    std::string_view result = FileUtil::getDirectory(filenamePath);

    AssertHelper::assertTrue(result == R"(C:\home\jean\)");
}

void FileUtilTest::relativePath() {
    std::string referenceDirectory = "xxx/yyy/zzz/www/";
    std::string path = "xxx/yyy/aaa/bbb/";

    std::string result = FileUtil::getRelativePath(referenceDirectory, path);

    AssertHelper::assertTrue(result == "../../aaa/bbb/");
}

void FileUtilTest::relativePathEqual() {
    std::string referenceDirectory = "xxx/yyy/";
    std::string path = "xxx/yyy/";

    std::string result = FileUtil::getRelativePath(referenceDirectory, path);

    AssertHelper::assertTrue(result.empty());
}

void FileUtilTest::simplifyDirectoryPathUnix() {
    std::string directoryPath = "xxx/yyy/../zzz/www/../rrr/";

    std::string result = FileUtil::simplifyDirectoryPath(directoryPath);

    AssertHelper::assertTrue(result == "xxx/zzz/rrr/");
}

void FileUtilTest::simplifyDirectoryPathWindows() {
    std::string directoryPath = R"(xxx\yyy\..\zzz\www\..\rrr\)";

    std::string result = FileUtil::simplifyDirectoryPath(directoryPath);

    AssertHelper::assertTrue(result == R"(xxx\zzz\rrr\)");
}

CppUnit::Test* FileUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("FileUtilTest");

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("getDirectoryUnix", &FileUtilTest::getDirectoryUnix));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("getDirectoryWindows", &FileUtilTest::getDirectoryWindows));

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePath", &FileUtilTest::relativePath));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePathEqual", &FileUtilTest::relativePathEqual));

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathUnix", &FileUtilTest::simplifyDirectoryPathUnix));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathWindows", &FileUtilTest::simplifyDirectoryPathWindows));

    return suite;
}
