#ifndef ENGINE_RESIZECONVEXHULL3DTEST_H
#define ENGINE_RESIZECONVEXHULL3DTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ResizeConvexHull3DTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void reduceBox();
		void tooMuchReduceBox();

		void nullExpandTetrahedron();
		void expandTetrahedron();
};

#endif
