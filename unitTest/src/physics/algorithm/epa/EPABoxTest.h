#ifndef ENGINE_EPABOXTEST_H
#define ENGINE_EPABOXTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPABoxTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void identicalAABBox();
		void overlapAABBox();

		void identicalOBBox();
		void overlapOnEdgeOBBox();
		void overlapOnCornerOBBox();

		void overlapOBBoxAABBox();
};

#endif
