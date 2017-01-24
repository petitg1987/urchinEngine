#ifndef ENGINE_TRIANGULATIONTEST_H
#define ENGINE_TRIANGULATIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class TriangulationTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void simpleTriangulation();
};

#endif
