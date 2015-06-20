#ifndef ENGINE_SHAPETOCONVEXOBJECTTEST_H
#define ENGINE_SHAPETOCONVEXOBJECTTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class ShapeToConvexObjectTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void boxConversion();
		void capsuleConversion();
		void cylinderConversion();
		void convexHullConversion();
		void sphereConversion();
};

#endif
