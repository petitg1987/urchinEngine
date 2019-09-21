#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"

#include "common/system/FileHandlerTest.h"
#include "AssertHelper.h"
using namespace urchin;

/**
 * Simplify path test (Unix version)
 */
void FileHandlerTest::simplifyDirectoryPathUnix()
{
	std::string directoryPath = "xxx/yyy/../zzz/www/../rrr/";
	std::string result = FileHandler::simplifyDirectoryPath(directoryPath);

	std::string expectedResult = "xxx/zzz/rrr/";
	AssertHelper::assertTrue(result == expectedResult);
}

/**
 * Simplify path test (Window version)
 */
void FileHandlerTest::simplifyDirectoryPathWindow()
{
	std::string directoryPath = R"(xxx\yyy\..\zzz\www\..\rrr\)";
	std::string result = FileHandler::simplifyDirectoryPath(directoryPath);

	std::string expectedResult = R"(xxx\zzz\rrr\)";
	AssertHelper::assertTrue(result == expectedResult);
}

void FileHandlerTest::relativePath()
{
	std::string referenceDirectory = "xxx/yyy/zzz/www/";
	std::string path = "xxx/yyy/aaa/bbb/";
	std::string result = FileHandler::getRelativePath(referenceDirectory, path);

	std::string expectedResult = "../../aaa/bbb/";
	AssertHelper::assertTrue(result == expectedResult);
}

void FileHandlerTest::relativePathEqual()
{
	std::string referenceDirectory = "xxx/yyy/";
	std::string path = "xxx/yyy/";
	std::string result = FileHandler::getRelativePath(referenceDirectory, path);

	AssertHelper::assertTrue(result.empty());
}

CppUnit::Test *FileHandlerTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("FileHandlerTest");

	suite->addTest(new CppUnit::TestCaller<FileHandlerTest>("simplifyDirectoryPathUnix", &FileHandlerTest::simplifyDirectoryPathUnix));
	suite->addTest(new CppUnit::TestCaller<FileHandlerTest>("simplifyDirectoryPathWindow", &FileHandlerTest::simplifyDirectoryPathWindow));

	suite->addTest(new CppUnit::TestCaller<FileHandlerTest>("relativePath", &FileHandlerTest::relativePath));
	suite->addTest(new CppUnit::TestCaller<FileHandlerTest>("relativePathEqual", &FileHandlerTest::relativePathEqual));

	return suite;
}
