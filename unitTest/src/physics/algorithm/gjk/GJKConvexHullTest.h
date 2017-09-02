#ifndef URCHINENGINE_GJKCONVEXHULLTEST_H
#define URCHINENGINE_GJKCONVEXHULLTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKConvexHullTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void identicalBox();
		void separateBox();
		void cornerInsideBox();

		void faceInsideTrapeze();

		void separateHexagon();
		void cornerInsideHexagon();
};

#endif
