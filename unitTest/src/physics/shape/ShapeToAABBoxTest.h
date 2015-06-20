#ifndef ENGINE_SHAPETOAABBOXTEST_H
#define ENGINE_SHAPETOAABBOXTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ShapeToAABBoxTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void boxConversion();
		void convexHullConversion();
};

#endif
