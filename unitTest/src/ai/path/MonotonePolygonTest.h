#ifndef ENGINE_MONOTONEPOLYGONTEST_H
#define ENGINE_MONOTONEPOLYGONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class MonotonePolygonTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void oneSplitVertex();
		void twoSplitVertex();
		void oneMergeVertex();
		void twoRegularVertex();
		void splitAndMergeVertex();

		void polygonOneHole();
		void polygonTwoHoles();
};

#endif
