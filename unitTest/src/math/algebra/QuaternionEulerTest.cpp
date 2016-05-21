#include <cppunit/extensions/HelperMacros.h>

#include "QuaternionEulerTest.h"
#include "math/algebra/MathValue.h"
#include "AssertHelper.h"
using namespace urchin;

void QuaternionEulerTest::eulerXYZ()
{
	Quaternion<float> quaternion(Vector3<float>(1.0, 3.141592653/4.0, 10.0), Quaternion<float>::RotationSequence::XYZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XYZ));
}

void QuaternionEulerTest::eulerXZY()
{
	Quaternion<float> quaternion(Vector3<float>(1.0, 3.141592653/4.0, 2.0), Quaternion<float>::RotationSequence::XZY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XZY));
}

void QuaternionEulerTest::eulerYXZ()
{
	Quaternion<float> quaternion(Vector3<float>(-1.0, 3.141592653/2.0, 2.0), Quaternion<float>::RotationSequence::YXZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YXZ));
}

void QuaternionEulerTest::eulerYZX()
{
	Quaternion<float> quaternion(Vector3<float>(3.141592653/2.0, -1.0, 2.0), Quaternion<float>::RotationSequence::YZX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YZX));
}

void QuaternionEulerTest::eulerZXY()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 3.141592653/2.0, 3.141592653/2.0), Quaternion<float>::RotationSequence::ZXY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZXY));
}

void QuaternionEulerTest::eulerZYX()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 0.0, 2.0), Quaternion<float>::RotationSequence::ZYX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZYX));
}

void QuaternionEulerTest::eulerXYX()
{
	Quaternion<float> quaternion(Vector3<float>(3.141592653/2.0, -1.0, 2.0), Quaternion<float>::RotationSequence::XYX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XYX));
}

void QuaternionEulerTest::eulerXZX()
{
	Quaternion<float> quaternion(Vector3<float>(3.141592653, -3.0, 3.141592653/4.0), Quaternion<float>::RotationSequence::XZX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XZX));
}

void QuaternionEulerTest::eulerYXY()
{
	Quaternion<float> quaternion(Vector3<float>(3.141592653, -3.0, 3.141592653/4.0), Quaternion<float>::RotationSequence::YXY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YXY));
}

void QuaternionEulerTest::eulerYZY()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 0.0, 2.0), Quaternion<float>::RotationSequence::YZY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YZY));
}

void QuaternionEulerTest::eulerZXZ()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 3.141592653/2.0, 3.141592653/2.0), Quaternion<float>::RotationSequence::ZXZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZXZ));
}

void QuaternionEulerTest::eulerZYZ()
{
	Quaternion<float> quaternion(Vector3<float>(3.141592653/2.0, -1.0, 3.141592653/2.0), Quaternion<float>::RotationSequence::ZYZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZYZ));
}

CppUnit::Test *QuaternionEulerTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("QuaternionTest");

	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerXYZ", &QuaternionEulerTest::eulerXYZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerXZY", &QuaternionEulerTest::eulerXZY));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerYXZ", &QuaternionEulerTest::eulerYXZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerYZX", &QuaternionEulerTest::eulerYZX));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerZXY", &QuaternionEulerTest::eulerZXY));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerZYX", &QuaternionEulerTest::eulerZYX));

	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerXYX", &QuaternionEulerTest::eulerXYX));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerXZX", &QuaternionEulerTest::eulerXZX));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerYXY", &QuaternionEulerTest::eulerYXY));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerYZY", &QuaternionEulerTest::eulerYZY));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerZXZ", &QuaternionEulerTest::eulerZXZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionEulerTest>("eulerZYZ", &QuaternionEulerTest::eulerZYZ));

	return suite;
}
