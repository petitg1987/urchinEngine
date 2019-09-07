#ifndef URCHINENGINE_INERTIACALCULATIONTEST_H
#define URCHINENGINE_INERTIACALCULATIONTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>

class InertiaCalculationTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void boxInertiaCalculation();
};

#endif
