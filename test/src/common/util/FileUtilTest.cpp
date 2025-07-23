#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>

#include "common/util/FileUtilTest.h"
#include "AssertHelper.h"
using namespace urchin;

void FileUtilTest::getDirectoryUnix() {
    std::string filenamePath = "/home/jean/binaryFile";

    std::string result = FileUtil::getDirectory(filenamePath);

    AssertHelper::assertTrue(result == "/home/jean/");
}

void FileUtilTest::getDirectoryWindows() {
    std::string filenamePath = R"(C:\home\jean\binaryFile)";

    std::string result = FileUtil::getDirectory(filenamePath);

    AssertHelper::assertTrue(result == R"(C:\home\jean\)");
}

void FileUtilTest::getFilesRecursiveWithSpecialChar() {
    std::vector<std::string> files = FileUtil::getFilesRecursive(FileSystem::instance().getResourcesDirectory() + "files/");

    AssertHelper::assertUnsignedIntEquals(files.size(), 1);
    AssertHelper::assertTrue(FileUtil::isFileExist(files[0]));
    AssertHelper::assertStringEquals(FileUtil::getFileName(files[0]), "file.txt");
}

CppUnit::Test* FileUtilTest::suite() {
    auto* suite = new CppUnit::TestSuite("FileUtilTest");

    suite->addTest(new CppUnit::TestCaller("getDirectoryUnix", &FileUtilTest::getDirectoryUnix));
    suite->addTest(new CppUnit::TestCaller("getDirectoryWindows", &FileUtilTest::getDirectoryWindows));

    suite->addTest(new CppUnit::TestCaller("getFilesRecursiveWithSpecialChar", &FileUtilTest::getFilesRecursiveWithSpecialChar));

    return suite;
}
