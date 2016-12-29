#ifndef ENGINE_RESIZECONVEXHULLTEST_H
#define ENGINE_RESIZECONVEXHULLTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ResizeConvexHullTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void reduceConvexHullBox();
};

#endif
