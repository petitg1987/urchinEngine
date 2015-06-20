#ifndef ENGINE_SHRINKCONVEXHULLTEST_H
#define ENGINE_SHRINKCONVEXHULLTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ShrinkConvexHullTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void shrinkConvexHullBox();
};

#endif
