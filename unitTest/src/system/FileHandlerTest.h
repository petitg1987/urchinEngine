#ifndef ENGINE_FILEHANDLERTEST_H
#define ENGINE_FILEHANDLERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class FileHandlerTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void simplifyDirectoryPathUnix();
		void simplifyDirectoryPathWindow();

		void relativePath();
		void relativePathEqual();
};

#endif
