#include <cppunit/extensions/HelperMacros.h>
#include <UrchinCommon.h>

#include "common/math/algorithm/MinEnclosingSphereTest.h"
#include "AssertHelper.h"
using namespace urchin;

void MinEnclosingSphereTest::twoPoints() {
    std::vector points = {Point3(0.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f)};

    Sphere<float> sphere = MinEnclosingSphere<float>().compute(points);

    AssertHelper::assertPoint3FloatEquals(sphere.getCenterOfMass(), Point3(0.5f, 0.0f, 0.0f), 0.01f);
    AssertHelper::assertFloatEquals(sphere.getRadius(), 0.5f, 0.01f);
}

void MinEnclosingSphereTest::frustumPoints() {
    Frustum frustum(58.71f, 1.777f, 0.1f, 2000.0f);
    std::array<Point3<float>, 8> pointsArray = frustum.getFrustumPoints();
    std::vector points(std::begin(pointsArray), std::end(pointsArray));

    Sphere<float> sphere = MinEnclosingSphere<float>().compute(points);

    AssertHelper::assertPoint3FloatEquals(sphere.getCenterOfMass(), Point3(0.0f, 0.0f, -2000.0f), 0.01f);
    AssertHelper::assertFloatEquals(sphere.getRadius(), 2293.674f, 0.01f);
}

void MinEnclosingSphereTest::rotatedFrustumPoints() {
    Frustum frustum(58.71f, 1.777f, 0.1f, 2000.0f);
    Matrix4<float> transformationMatrix = Matrix4<float>::buildTranslation(10.0f, 0.0f, 0.0f) * Matrix4<float>::buildRotationY(1.0f) * Matrix4<float>::buildRotationX(0.05f);
    std::array<Point3<float>, 8> pointsArray = (transformationMatrix * frustum).getFrustumPoints();
    std::vector points(std::begin(pointsArray), std::end(pointsArray));

    Sphere<float> sphere = MinEnclosingSphere<float>().compute(points);

    AssertHelper::assertPoint3FloatEquals(sphere.getCenterOfMass(), Point3(-1670.84f, 99.9583f, -1079.25f), 0.01f);
    AssertHelper::assertFloatEquals(sphere.getRadius(), 2293.67431f, 0.01f);
}

CppUnit::Test* MinEnclosingSphereTest::suite() {
    auto* suite = new CppUnit::TestSuite("MinEnclosingSphereTest");

    suite->addTest(new CppUnit::TestCaller("twoPoints", &MinEnclosingSphereTest::twoPoints));
    suite->addTest(new CppUnit::TestCaller("frustumPoints", &MinEnclosingSphereTest::frustumPoints));
    suite->addTest(new CppUnit::TestCaller("rotatedFrustumPoints", &MinEnclosingSphereTest::rotatedFrustumPoints));

    return suite;
}