#ifndef ENGINE_POLYGONSUNIONTEST_H
#define ENGINE_POLYGONSUNIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class PolygonsUnionTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void twoPolygonsNoUnion();
		void polygonInsideAnother();
		void twoPolygonsUnion();
		void threePolygonsUnion();
		void twoPolygonsUnionAndSeparatePolygon();

};

#endif
