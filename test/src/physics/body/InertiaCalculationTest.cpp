#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/body/InertiaCalculationTest.h"
using namespace urchin;

void InertiaCalculationTest::boxInertiaCalculation() {
    const float mass = 10.0f;
    CollisionBoxShape boxShape(Vector3<float>(1.0f, 2.0f, 3.0f));
    PhysicsTransform transformRotateXAxis90(Point3<float>(0.0f, 0.0f, 0.0f), Quaternion<float>(Vector3<float>(1.0f, 0.0f, 0.0f), -MathValue::PI_FLOAT / 2.0f));

    Vector3<float> localInertia = boxShape.computeLocalInertia(mass);
    Vector3<float> invLocalInertia = Vector3<float>(1.0f/localInertia.X, 1.0f/localInertia.Y, 1.0f/localInertia.Z);

    Matrix3<float> invWorldInertia = InertiaCalculation::computeInverseWorldInertia(invLocalInertia, transformRotateXAxis90);

    AssertHelper::assertFloatEquals(invWorldInertia(0, 0), 0.02307692307f);
    AssertHelper::assertFloatEquals(invWorldInertia(0, 1), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(0, 2), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 0), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 1), 0.06f);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 2), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 0), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 1), 0.0f);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 2), 0.03f);
}

CppUnit::Test* InertiaCalculationTest::suite() {
    auto* suite = new CppUnit::TestSuite("InertiaCalculationTest");

    suite->addTest(new CppUnit::TestCaller<InertiaCalculationTest>("boxInertiaCalculation", &InertiaCalculationTest::boxInertiaCalculation));

    return suite;
}
