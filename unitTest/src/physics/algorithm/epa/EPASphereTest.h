#ifndef ENGINE_EPASPHERETEST_H
#define ENGINE_EPASPHERETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPASphereTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void identicalSphere();
		void overlapSphere();
};

#endif
