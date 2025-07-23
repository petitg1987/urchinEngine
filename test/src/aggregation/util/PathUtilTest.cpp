#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinAggregation.h>

#include "aggregation/util/PathUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void PathUtilTest::relativePath() {
    std::string referenceDirectory = "/xxx/yyy/zzz/www/";
    std::string path = "/xxx/yyy/aaa/bbb/";

    std::string result = PathUtil::computeRelativePath(referenceDirectory, path);

    AssertHelper::assertTrue(result == "../../aaa/bbb/");
}

void PathUtilTest::relativePathEqual() {
    std::string referenceDirectory = "/xxx/yyy/";
    std::string path = "/xxx/yyy/";

    std::string result = PathUtil::computeRelativePath(referenceDirectory, path);

    AssertHelper::assertTrue(result.empty());
}

void PathUtilTest::relativePathOnDifferentDisk() {
    std::string referenceDirectory = "C:\\xxx\\yyy";
    std::string path = "D:\\zzz";

    bool exceptionCaught = false;
    try {
        PathUtil::computeRelativePath(referenceDirectory, path);
    } catch (const std::exception&) {
        exceptionCaught = true;
    }

    AssertHelper::assertTrue(exceptionCaught);
}

void PathUtilTest::simplifyDirectoryPathUnix() {
    std::string directoryPath = "xxx/yyy/../zzz/www/../rrr/";

    std::string result = PathUtil::simplifyDirectoryPath(directoryPath);

    AssertHelper::assertTrue(result == "xxx/zzz/rrr/");
}

void PathUtilTest::simplifyDirectoryPathWindows() {
    std::string directoryPath = R"(xxx\yyy\..\zzz\www\..\rrr\)";

    std::string result = PathUtil::simplifyDirectoryPath(directoryPath);

    AssertHelper::assertTrue(result == R"(xxx\zzz\rrr\)");
}

CppUnit::Test* PathUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("PathUtilTest");

    suite->addTest(new CppUnit::TestCaller("relativePath", &PathUtilTest::relativePath));
    suite->addTest(new CppUnit::TestCaller("relativePathEqual", &PathUtilTest::relativePathEqual));
    suite->addTest(new CppUnit::TestCaller("relativePathOnDifferentDisk", &PathUtilTest::relativePathOnDifferentDisk));

    suite->addTest(new CppUnit::TestCaller("simplifyDirectoryPathUnix", &PathUtilTest::simplifyDirectoryPathUnix));
    suite->addTest(new CppUnit::TestCaller("simplifyDirectoryPathWindows", &PathUtilTest::simplifyDirectoryPathWindows));

    return suite;
}
