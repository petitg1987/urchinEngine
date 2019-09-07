#ifndef URCHINENGINE_TRIANGULATIONTEST_H
#define URCHINENGINE_TRIANGULATIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class TriangulationTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void triangleTriangulation();
		void cubeTriangulation();
		void twoNearPoints();
		void threeAlignedPoints();
		void alternationPoints();

		void cavityTriangulation1();
		void cavityTriangulation2();

		void twoMonotonePolygons();
		void threeMonotonePolygons();
};

#endif
