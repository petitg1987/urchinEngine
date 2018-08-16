#include <cppunit/extensions/HelperMacros.h>

#include "QuaternionTest.h"
#include "math/algebra/MathValue.h"
#include "AssertHelper.h"
using namespace urchin;

void QuaternionTest::eulerXYZ()
{
	Quaternion<float> quaternion(Vector3<float>(1.0, PI_VALUE/4.0, 10.0), Quaternion<float>::RotationSequence::XYZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XYZ));
}

void QuaternionTest::eulerXZY()
{
	Quaternion<float> quaternion(Vector3<float>(1.0, PI_VALUE/4.0, 2.0), Quaternion<float>::RotationSequence::XZY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XZY));
}

void QuaternionTest::eulerYXZ()
{
	Quaternion<float> quaternion(Vector3<float>(-1.0, PI_VALUE/2.0, 2.0), Quaternion<float>::RotationSequence::YXZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YXZ));
}

void QuaternionTest::eulerYZX()
{
	Quaternion<float> quaternion(Vector3<float>(PI_VALUE/2.0, -1.0, 2.0), Quaternion<float>::RotationSequence::YZX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YZX));
}

void QuaternionTest::eulerZXY()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, PI_VALUE/2.0, PI_VALUE/2.0), Quaternion<float>::RotationSequence::ZXY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZXY));
}

void QuaternionTest::eulerZYX()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 0.0, 2.0), Quaternion<float>::RotationSequence::ZYX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZYX));
}

void QuaternionTest::eulerXYX()
{
	Quaternion<float> quaternion(Vector3<float>(PI_VALUE/2.0, -1.0, 2.0), Quaternion<float>::RotationSequence::XYX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XYX));
}

void QuaternionTest::eulerXZX()
{
	Quaternion<float> quaternion(Vector3<float>(PI_VALUE, -3.0, PI_VALUE/4.0), Quaternion<float>::RotationSequence::XZX);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZX);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::XZX));
}

void QuaternionTest::eulerYXY()
{
	Quaternion<float> quaternion(Vector3<float>(PI_VALUE, -3.0, PI_VALUE/4.0), Quaternion<float>::RotationSequence::YXY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YXY));
}

void QuaternionTest::eulerYZY()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, 0.0, 2.0), Quaternion<float>::RotationSequence::YZY);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZY);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::YZY));
}

void QuaternionTest::eulerZXZ()
{
	Quaternion<float> quaternion(Vector3<float>(0.0, PI_VALUE/2.0, PI_VALUE/2.0), Quaternion<float>::RotationSequence::ZXZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZXZ));
}

void QuaternionTest::eulerZYZ()
{
	Quaternion<float> quaternion(Vector3<float>(PI_VALUE/2.0, -1.0, PI_VALUE/2.0), Quaternion<float>::RotationSequence::ZYZ);
	Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYZ);

	AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>(eulerAngle, Quaternion<float>::RotationSequence::ZYZ));
}

void QuaternionTest::slerp50Rotation()
{
    Quaternion<float> q1(Vector3<float>(0.0, 1.0, 0.0), 0.0);
    Quaternion<float> q2(Vector3<float>(0.0, 1.0, 0.0), 3.14159265359/2.0);
    Quaternion<float> slerpQuaternion = q1.slerp(q2, 0.5);

    Vector3<float> axis;
    float angle;
    slerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3<float>(0.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(angle, 3.14159265359/4.0);
}

void QuaternionTest::slerp25Rotation()
{
    Quaternion<float> q1(Vector3<float>(0.0, 1.0, 0.0), 0.0);
    Quaternion<float> q2(Vector3<float>(0.0, 1.0, 0.0), 3.14159265359/2.0);
    Quaternion<float> slerpQuaternion = q1.slerp(q2, 0.25);

    Vector3<float> axis;
    float angle;
    slerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3<float>(0.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(angle, 3.14159265359/8.0);
}

void QuaternionTest::lerp50Rotation()
{
	Quaternion<float> q1(Vector3<float>(0.0, 1.0, 0.0), 0.0);
	Quaternion<float> q2(Vector3<float>(0.0, 1.0, 0.0), 3.14159265359/2.0);
	Quaternion<float> lerpQuaternion = q1.lerp(q2, 0.5);

	Vector3<float> axis;
	float angle;
	lerpQuaternion.toAxisAngle(axis, angle);

	AssertHelper::assertVector3FloatEquals(axis, Vector3<float>(0.0, 1.0, 0.0));
	AssertHelper::assertFloatEquals(angle, 3.14159265359/4.0);
}

void QuaternionTest::lerp25Rotation()
{
    Quaternion<float> q1(Vector3<float>(0.0, 1.0, 0.0), 0.0);
    Quaternion<float> q2(Vector3<float>(0.0, 1.0, 0.0), 3.14159265359/2.0);
    Quaternion<float> lerpQuaternion = q1.lerp(q2, 0.25);

    Vector3<float> axis;
    float angle;
    lerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3<float>(0.0, 1.0, 0.0));
    AssertHelper::assertFloatEquals(angle, 0.3769595623);
}

CppUnit::Test *QuaternionTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("QuaternionTest");

	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerXYZ", &QuaternionTest::eulerXYZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerXZY", &QuaternionTest::eulerXZY));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerYXZ", &QuaternionTest::eulerYXZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerYZX", &QuaternionTest::eulerYZX));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerZXY", &QuaternionTest::eulerZXY));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerZYX", &QuaternionTest::eulerZYX));

	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerXYX", &QuaternionTest::eulerXYX));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerXZX", &QuaternionTest::eulerXZX));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerYXY", &QuaternionTest::eulerYXY));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerYZY", &QuaternionTest::eulerYZY));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerZXZ", &QuaternionTest::eulerZXZ));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("eulerZYZ", &QuaternionTest::eulerZYZ));

	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("slerp50Rotation", &QuaternionTest::slerp50Rotation));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("slerp25Rotation", &QuaternionTest::slerp50Rotation));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("lerp50Rotation", &QuaternionTest::lerp50Rotation));
	suite->addTest(new CppUnit::TestCaller<QuaternionTest>("lerp25Rotation", &QuaternionTest::lerp25Rotation));

	return suite;
}
