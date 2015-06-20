#ifndef ENGINE_SWEEPANDPRUNETEST_H
#define ENGINE_SWEEPANDPRUNETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class SweepAndPruneTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void moveBodyCollide();
		void moveBodyNotCollide();
};

#endif
