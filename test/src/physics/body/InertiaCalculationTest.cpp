#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/body/InertiaCalculationTest.h"
using namespace urchin;

void InertiaCalculationTest::boxInertiaCalculation()
{
    const float mass = 10.0f;
    CollisionBoxShape boxShape(Vector3<float>(1.0, 2.0, 3.0));
    PhysicsTransform transformRotateXAxis90(Point3<float>(0.0, 0.0, 0.0), Quaternion<float>(Vector3<float>(1.0, 0.0, 0.0), -PI_VALUE/2));

    Vector3<float> localInertia = boxShape.computeLocalInertia(mass);
    Vector3<float> invLocalInertia = Vector3<float>(1.0f/localInertia.X, 1.0f/localInertia.Y, 1.0f/localInertia.Z);

    Matrix3<float> invWorldInertia = InertiaCalculation::computeInverseWorldInertia(invLocalInertia, transformRotateXAxis90);

    AssertHelper::assertFloatEquals(invWorldInertia(0, 0), 0.02307692307);
    AssertHelper::assertFloatEquals(invWorldInertia(0, 1), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(0, 2), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 0), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 1), 0.06);
    AssertHelper::assertFloatEquals(invWorldInertia(1, 2), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 0), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 1), 0.0);
    AssertHelper::assertFloatEquals(invWorldInertia(2, 2), 0.03);
}

CppUnit::Test *InertiaCalculationTest::suite()
{
    auto *suite = new CppUnit::TestSuite("InertiaCalculationTest");

    suite->addTest(new CppUnit::TestCaller<InertiaCalculationTest>("boxInertiaCalculation", &InertiaCalculationTest::boxInertiaCalculation));

    return suite;
}
