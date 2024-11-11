#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/shape/ShapeToAABBoxTest.h>
using namespace urchin;

void ShapeToAABBoxTest::boxConversion() {
    CollisionBoxShape collisionBox(Vector3(1.0f, 2.0f, 1.0f)); //box 1x2x1
    PhysicsTransform transform(Point3(0.0f, 0.0f, 0.0f), //move 0 unit on X, Y and Z axis
            Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 4.0f)); //rotate 45° on Z axis

    AABBox<float> box = collisionBox.toAABBox(transform);

    AssertHelper::assertFloatEquals(box.getHalfSize(0), 2.12132034356f);
    AssertHelper::assertFloatEquals(box.getHalfSize(1), 2.12132034356f);
    AssertHelper::assertFloatEquals(box.getHalfSize(2), 1.0f);
    AssertHelper::assertPoint3FloatEquals(box.getMin(), Point3(-2.12132034356f, -2.12132034356f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(box.getMax(), Point3(2.12132034356f, 2.12132034356f, 1.0f));
}

void ShapeToAABBoxTest::coneConversion() {
    CollisionConeShape collisionCone(1.0f, 4.0f, ConeShape<float>::CONE_Y_NEGATIVE);
    PhysicsTransform transform(Point3(1.0f, 5.0f, 0.0f), //move 1 unit on X and 5 unit on Y
                Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 2.0f)); //rotate 90° on Z axis

    AABBox<float> box = collisionCone.toAABBox(transform);

    AssertHelper::assertFloatEquals(box.getHalfSize(0), 2.0f);
    AssertHelper::assertFloatEquals(box.getHalfSize(1), 1.0f);
    AssertHelper::assertFloatEquals(box.getHalfSize(2), 1.0f);
    AssertHelper::assertPoint3FloatEquals(box.getMin(), Point3(-2.0f, 4.0f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(box.getMax(), Point3(2.0f, 6.0f, 1.0f));
}

void ShapeToAABBoxTest::convexHullConversion() {
    std::vector<Point3<float>> boxPoints = {
            Point3(-1.0f, -2.0f, 1.0f), Point3(1.0f, -2.0f, 1.0f), Point3(1.0f, 2.0f, 1.0f), Point3(-1.0f, 2.0f, 1.0f),
            Point3(-1.0f, -2.0f, -1.0f), Point3(1.0f, -2.0f, -1.0f), Point3(1.0f, 2.0f, -1.0f), Point3(-1.0f, 2.0f, -1.0f),
    };

    CollisionConvexHullShape collisionConvexHull(boxPoints);
    PhysicsTransform transform(Point3(0.0f, 0.0f, 0.0f), //move 0 unit on X, Y and Z axis
            Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 4.0f)); //rotate 45° on Z axis

    AABBox<float> box = collisionConvexHull.toAABBox(transform);

    AssertHelper::assertFloatEquals(box.getHalfSize(0), 2.12132034356f);
    AssertHelper::assertFloatEquals(box.getHalfSize(1), 2.12132034356f);
    AssertHelper::assertFloatEquals(box.getHalfSize(2), 1.0f);
    AssertHelper::assertPoint3FloatEquals(box.getMin(), Point3(-2.12132034356f, -2.12132034356f, -1.0f));
    AssertHelper::assertPoint3FloatEquals(box.getMax(), Point3(2.12132034356f, 2.12132034356f, 1.0f));
}

CppUnit::Test* ShapeToAABBoxTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShapeToAABBoxTest");

    suite->addTest(new CppUnit::TestCaller<ShapeToAABBoxTest>("boxConversion", &ShapeToAABBoxTest::boxConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToAABBoxTest>("coneConversion", &ShapeToAABBoxTest::coneConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToAABBoxTest>("convexHullConversion", &ShapeToAABBoxTest::convexHullConversion));

    return suite;
}
