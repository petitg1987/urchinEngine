#ifndef ENGINE_QUATERNIONEULERTEST_H
#define ENGINE_QUATERNIONEULERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/Test.h>
#include "UrchinCommon.h"

class QuaternionEulerTest : public CppUnit::TestFixture
{
	public:
		static CppUnit::Test *suite();

		void eulerXYZ();
		void eulerXZY();
		void eulerYXZ();
		void eulerYZX();
		void eulerZXY();
		void eulerZYX();

		void eulerXYX();
		void eulerXZX();
		void eulerYXY();
		void eulerYZY();
		void eulerZXZ();
		void eulerZYZ();
};

#endif
