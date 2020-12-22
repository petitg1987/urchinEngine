#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "FileUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

/**
 * Simplify path test (Unix version)
 */
void FileUtilTest::simplifyDirectoryPathUnix() {
    std::string directoryPath = "xxx/yyy/../zzz/www/../rrr/";
    std::string result = FileUtil::simplifyDirectoryPath(directoryPath);

    std::string expectedResult = "xxx/zzz/rrr/";
    AssertHelper::assertTrue(result == expectedResult);
}

/**
 * Simplify path test (Window version)
 */
void FileUtilTest::simplifyDirectoryPathWindow() {
    std::string directoryPath = R"(xxx\yyy\..\zzz\www\..\rrr\)";
    std::string result = FileUtil::simplifyDirectoryPath(directoryPath);

    std::string expectedResult = R"(xxx\zzz\rrr\)";
    AssertHelper::assertTrue(result == expectedResult);
}

void FileUtilTest::relativePath() {
    std::string referenceDirectory = "xxx/yyy/zzz/www/";
    std::string path = "xxx/yyy/aaa/bbb/";
    std::string result = FileUtil::getRelativePath(referenceDirectory, path);

    std::string expectedResult = "../../aaa/bbb/";
    AssertHelper::assertTrue(result == expectedResult);
}

void FileUtilTest::relativePathEqual() {
    std::string referenceDirectory = "xxx/yyy/";
    std::string path = "xxx/yyy/";
    std::string result = FileUtil::getRelativePath(referenceDirectory, path);

    AssertHelper::assertTrue(result.empty());
}

CppUnit::Test* FileUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("FileUtilTest");

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathUnix", &FileUtilTest::simplifyDirectoryPathUnix));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("simplifyDirectoryPathWindow", &FileUtilTest::simplifyDirectoryPathWindow));

    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePath", &FileUtilTest::relativePath));
    suite->addTest(new CppUnit::TestCaller<FileUtilTest>("relativePathEqual", &FileUtilTest::relativePathEqual));

    return suite;
}
