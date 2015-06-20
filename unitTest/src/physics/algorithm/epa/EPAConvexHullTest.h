#ifndef ENGINE_EPACONVEXHULLTEST_H
#define ENGINE_EPACONVEXHULLTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPAConvexHullTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void identicalBox();
		void cornerInsideBox();

		void faceInsideTrapeze();

		void cornerInsideHexagon();
};

#endif
