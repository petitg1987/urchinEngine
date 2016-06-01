#ifndef ENGINE_VORONOIREGIONTEST_H
#define ENGINE_VORONOIREGIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include "UrchinCommon.h"

class ClosestPointTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void closestPointLineSegment2D();
		void closestPointLineSegment3D();

		void closestPointTriangle3D();

		void closestPointTetrahedron();
};

#endif
