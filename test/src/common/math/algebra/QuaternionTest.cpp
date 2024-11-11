#include <cppunit/extensions/HelperMacros.h>

#include <common/math/algebra/QuaternionTest.h>
#include <math/algebra/MathValue.h>
#include <AssertHelper.h>
using namespace urchin;

void QuaternionTest::multiplyAxisAngleQuaternions9And45() {
    Quaternion<float> axisAngle09DegreeQuaternion = Quaternion<float>::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), MathValue::PI_FLOAT / 20.0f);
    Quaternion<float> axisAngle45DegreeQuaternion = Quaternion<float>::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), MathValue::PI_FLOAT / 4.0f);
    Quaternion<float> totalRotation = axisAngle09DegreeQuaternion * axisAngle45DegreeQuaternion;

    Vector3<float> axis;
    float angle;
    totalRotation.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 20.0f + MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::multiplyLookAtQuaternions9And45() {
    Quaternion<float> lookAt09DegreeQuaternion = Quaternion<float>::lookUp(Vector3<float>(std::asin(MathValue::PI_FLOAT / 20.0f), 0.0f, 1.0f).normalize(), Vector3(0.0f, 1.0f, 0.0f));
    Quaternion<float> lookAt45DegreeQuaternion = Quaternion<float>::lookUp(Vector3(0.70710678118f, 0.0f, 0.70710678118f), Vector3(0.0f, 1.0f, 0.0f));

    Quaternion<float> totalRotation = lookAt09DegreeQuaternion * lookAt45DegreeQuaternion;
    Vector3<float> axis;
    float angle;
    totalRotation.toAxisAngle(axis, angle);
    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 20.0f + MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::multiplyLookAtQuaternions180And45() {
    Quaternion<float> lookAt180DegreeQuaternion = Quaternion<float>::lookUp(Vector3(0.0f, 0.0f, -1.0f).normalize(), Vector3(0.0f, 1.0f, 0.0f));
    Quaternion<float> lookAt45DegreeQuaternion = Quaternion<float>::lookUp(Vector3(0.70710678118f, 0.0f, 0.70710678118f), Vector3(0.0f, 1.0f, 0.0f));

    Quaternion<float> totalRotation = lookAt180DegreeQuaternion * lookAt45DegreeQuaternion;
    Vector3<float> axis;
    float angle;
    totalRotation.toAxisAngle(axis, angle);
    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT + MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::multiplyLookAtQuaternions180And45UpDown() {
    Quaternion<float> lookAt0DegreeQuaternion = Quaternion<float>::lookUp(Vector3(0.0f, 0.0f, -1.0f).normalize(), Vector3(0.0f, -1.0f, 0.0f));
    Quaternion<float> lookAt45DegreeQuaternion = Quaternion<float>::lookUp(Vector3(0.70710678118f, 0.0f, 0.70710678118f), Vector3(0.0f, -1.0f, 0.0f));

    Quaternion<float> totalRotation = lookAt0DegreeQuaternion * lookAt45DegreeQuaternion;
    Vector3<float> axis;
    float angle;
    totalRotation.toAxisAngle(axis, angle);
    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, -1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT + MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::rotationFrom90AndTo135() {
    Vector3<float> v90Degree(1.0f, 0.0f, 0.0);
    Vector3<float> v135Degree(0.70710678118f, 0.0, -0.70710678118f);

    Quaternion<float> rotation = Quaternion<float>::rotationFromTo(v90Degree, v135Degree);

    Vector3<float> result = rotation.normalize().rotateVector(v90Degree);
    AssertHelper::assertVector3FloatEquals(result, v135Degree);
}

void QuaternionTest::rotationFrom90AndTo90() {
    Vector3 v90Degree(1.0f, 0.0f, 0.0f);

    Quaternion<float> rotation = Quaternion<float>::rotationFromTo(v90Degree, v90Degree);

    Vector3<float> result = rotation.normalize().rotateVector(v90Degree);
    AssertHelper::assertVector3FloatEquals(result, v90Degree);
}

void QuaternionTest::rotationFromMinus90AndTo90() {
    Vector3 vMinus90Degree(-1.0f, 0.0f, 0.0f);
    Vector3 v90Degree(1.0f, 0.0f, 0.0f);

    Quaternion<float> rotation = Quaternion<float>::rotationFromTo(vMinus90Degree, v90Degree);

    Vector3<float> result = rotation.normalize().rotateVector(vMinus90Degree);
    AssertHelper::assertVector3FloatEquals(result, v90Degree);
}

void QuaternionTest::eulerXYZ() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(1.0f, MathValue::PI_FLOAT / 4.0f, 10.0f), Quaternion<float>::RotationSequence::XYZ);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYZ);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::XYZ));
}

void QuaternionTest::eulerXZY() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(1.0f, MathValue::PI_FLOAT / 4.0f, 2.0f), Quaternion<float>::RotationSequence::XZY);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZY);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::XZY));
}

void QuaternionTest::eulerYXZ() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(-1.0f, MathValue::PI_FLOAT / 2.0f, 2.0f), Quaternion<float>::RotationSequence::YXZ);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXZ);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::YXZ));
}

void QuaternionTest::eulerYZX() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3<float>(MathValue::PI_FLOAT / 2.0f, -1.0f, 2.0f), Quaternion<float>::RotationSequence::YZX);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZX);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::YZX));
}

void QuaternionTest::eulerZXY() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(0.0f, MathValue::PI_FLOAT / 2.0f, MathValue::PI_FLOAT / 2.0f), Quaternion<float>::RotationSequence::ZXY);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXY);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::ZXY));
}

void QuaternionTest::eulerZYX() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(0.0f, 0.0f, 2.0f), Quaternion<float>::RotationSequence::ZYX);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYX);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::ZYX));
}

void QuaternionTest::eulerXYX() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3<float>(MathValue::PI_FLOAT / 2.0f, -1.0f, 2.0f), Quaternion<float>::RotationSequence::XYX);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XYX);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::XYX));
}

void QuaternionTest::eulerXZX() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3<float>(MathValue::PI_FLOAT, -3.0f, MathValue::PI_FLOAT / 4.0f), Quaternion<float>::RotationSequence::XZX);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::XZX);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::XZX));
}

void QuaternionTest::eulerYXY() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3<float>(MathValue::PI_FLOAT, -3.0f, MathValue::PI_FLOAT / 4.0f), Quaternion<float>::RotationSequence::YXY);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YXY);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::YXY));
}

void QuaternionTest::eulerYZY() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(0.0f, 0.0f, 2.0f), Quaternion<float>::RotationSequence::YZY);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::YZY);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::YZY));
}

void QuaternionTest::eulerZXZ() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3(0.0f, MathValue::PI_FLOAT / 2.0f, MathValue::PI_FLOAT / 2.0f), Quaternion<float>::RotationSequence::ZXZ);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZXZ);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::ZXZ));
}

void QuaternionTest::eulerZYZ() {
    Quaternion<float> quaternion = Quaternion<float>::fromEuler(Vector3<float>(MathValue::PI_FLOAT / 2.0f, -1.0f, MathValue::PI_FLOAT / 2.0f), Quaternion<float>::RotationSequence::ZYZ);
    Vector3<float> eulerAngle = quaternion.toEulerAngle(Quaternion<float>::RotationSequence::ZYZ);

    AssertHelper::assertQuaternionFloatEquals(quaternion, Quaternion<float>::fromEuler(eulerAngle, Quaternion<float>::RotationSequence::ZYZ));
}

void QuaternionTest::slerp50Rotation() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.0f);
    Quaternion<float> q2 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    Quaternion<float> slerpQuaternion = q1.slerp(q2, 0.5f);

    Vector3<float> axis;
    float angle;
    slerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::slerp25Rotation() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.0f);
    Quaternion<float> q2 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    Quaternion<float> slerpQuaternion = q1.slerp(q2, 0.25f);

    Vector3<float> axis;
    float angle;
    slerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 8.0f);
}

void QuaternionTest::slerpShortestPath() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.00345267f); //0.2 degrees
    Quaternion<float> q2 = Quaternion<float>::rotationY(4.18863f); //240 degrees
    Quaternion<float> slerpQuaternion = q1.slerp(q2, 0.01f);

    Vector3<float> axis;
    float angle;
    slerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, -1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, 0.0175102f);
}

void QuaternionTest::lerp50Rotation() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.0f);
    Quaternion<float> q2 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    Quaternion<float> lerpQuaternion = q1.lerp(q2, 0.5f);

    Vector3<float> axis;
    float angle;
    lerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 4.0f);
}

void QuaternionTest::lerp25Rotation() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.0f);
    Quaternion<float> q2 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    Quaternion<float> lerpQuaternion = q1.lerp(q2, 0.25f);

    Vector3<float> axis;
    float angle;
    lerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, 0.3769595623f);
}

void QuaternionTest::lerpShortestPath() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(0.00345267f); //0.2 degrees
    Quaternion<float> q2 = Quaternion<float>::rotationY(4.18863f); //240 degrees
    Quaternion<float> lerpQuaternion = q1.lerp(q2, 0.01f);

    Vector3<float> axis;
    float angle;
    lerpQuaternion.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, -1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, 0.0139822755f);
}

void QuaternionTest::toAxisAngle90() {
    Quaternion<float> q = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);

    Vector3<float> axis;
    float angle;
    q.toAxisAngle(axis, angle);

    AssertHelper::assertVector3FloatEquals(axis, Vector3(0.0f, 1.0f, 0.0f));
    AssertHelper::assertFloatEquals(angle, MathValue::PI_FLOAT / 2.0f);
}

void QuaternionTest::equalOrientationAndRotation() {
    Quaternion<float> q1 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    Quaternion<float> q2 = Quaternion<float>::rotationY(-2.0f * MathValue::PI_FLOAT + MathValue::PI_FLOAT / 2.0f);
    AssertHelper::assertTrue(q1.isEqualOrientation(q2, 0.01f));
    AssertHelper::assertTrue(!q1.isEqualRotation(q2, 0.01f));

    q1 = Quaternion<float>::rotationY(-MathValue::PI_FLOAT / 2.0f);
    q2 = Quaternion<float>::rotationY(MathValue::PI_FLOAT / 2.0f);
    AssertHelper::assertTrue(!q1.isEqualOrientation(q2, 0.01f));
    AssertHelper::assertTrue(!q1.isEqualRotation(q2, 0.01f));

    q1 = Quaternion<float>::rotationY(0.0f);
    q2 = Quaternion<float>::rotationY(2.0f * MathValue::PI_FLOAT);
    AssertHelper::assertTrue(q1.isEqualOrientation(q2, 0.01f));
    AssertHelper::assertTrue(!q1.isEqualRotation(q2, 0.01f));

    q1 = Quaternion<float>::fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), MathValue::PI_FLOAT / 2.0f);
    q2 = Quaternion<float>::fromAxisAngle(Vector3(0.0f, -1.0f, 0.0f), -MathValue::PI_FLOAT / 2.0f);
    AssertHelper::assertTrue(q1.isEqualOrientation(q2, 0.01f));
    AssertHelper::assertTrue(q1.isEqualRotation(q2, 0.01f));

    q1 = Quaternion<float>::fromAxisAngle(Vector3(2.0f, 1.0f, 0.0f).normalize(), MathValue::PI_FLOAT / 2.0f);
    q2 = Quaternion<float>::fromAxisAngle(Vector3(2.0f, 1.0f, 0.0f).normalize(), -MathValue::PI_FLOAT / 2.0f + 0.2f);
    AssertHelper::assertTrue(!q1.isEqualOrientation(q2, 0.01f));
    AssertHelper::assertTrue(!q1.isEqualRotation(q2, 0.01f));
}

CppUnit::Test* QuaternionTest::suite() {
    auto* suite = new CppUnit::TestSuite("QuaternionTest");

    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("multiplyAxisAngleQuaternions9And45", &QuaternionTest::multiplyAxisAngleQuaternions9And45));

    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("multiplyLookAtQuaternions9And45", &QuaternionTest::multiplyLookAtQuaternions9And45));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("multiplyLookAtQuaternions180And45", &QuaternionTest::multiplyLookAtQuaternions180And45));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("multiplyLookAtQuaternions180And45UpDown", &QuaternionTest::multiplyLookAtQuaternions180And45UpDown));

    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("rotationFrom90AndTo135", &QuaternionTest::rotationFrom90AndTo135));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("rotationFrom90AndTo90", &QuaternionTest::rotationFrom90AndTo90));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("rotationFromMinus90AndTo90", &QuaternionTest::rotationFromMinus90AndTo90));

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
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("slerpShortestPath", &QuaternionTest::slerpShortestPath));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("lerp50Rotation", &QuaternionTest::lerp50Rotation));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("lerp25Rotation", &QuaternionTest::lerp25Rotation));
    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("lerpShortestPath", &QuaternionTest::lerpShortestPath));

    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("toAxisAngle90", &QuaternionTest::toAxisAngle90));

    suite->addTest(new CppUnit::TestCaller<QuaternionTest>("equalOrientationAndRotation", &QuaternionTest::equalOrientationAndRotation));

    return suite;
}
