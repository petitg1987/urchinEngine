#ifndef URCHINENGINE_GJKCONVEXOBJECTTEST_H
#define URCHINENGINE_GJKCONVEXOBJECTTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class GJKConvexObjectTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void separateSphereAndBox();
		void overlapSphereAndBox();
};

#endif
