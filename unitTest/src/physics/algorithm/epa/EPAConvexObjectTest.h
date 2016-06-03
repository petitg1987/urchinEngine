#ifndef ENGINE_EPACONVEXOBJECTTEST_H
#define ENGINE_EPACONVEXOBJECTTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class EPAConvexObjectTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void overlapSphereAndBox();
		void lightlyContactBoxAndCapsule();
		void lightlyContactBoxAndCapsule2();
};

#endif
