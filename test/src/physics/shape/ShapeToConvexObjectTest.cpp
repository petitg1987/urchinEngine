#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <UrchinCommon.h>
#include <UrchinPhysicsEngine.h>

#include <AssertHelper.h>
#include <physics/shape/ShapeToConvexObjectTest.h>
using namespace urchin;

void ShapeToConvexObjectTest::boxConversion() {
    CollisionBoxShape collisionBox(Vector3<float>(1.0f, 2.0f, 1.0f)); //box 1x2x1
    PhysicsTransform transform(Point3<float>(1.0f, 1.0f, 1.0f), //move 1 unit on X, Y and Z axis
            Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f)); //rotate 90° on X axis

    auto boxObject = collisionBox.toConvexObject(transform);
    const auto* box = dynamic_cast<CollisionBoxObject*>(boxObject.get());

    AssertHelper::assertPoint3FloatEquals(box->getCenterOfMass(), Point3<float>(1.0f, 1.0f, 1.0f));
    AssertHelper::assertFloatEquals(box->getHalfSize(0), 1.0f);
    AssertHelper::assertFloatEquals(box->getHalfSize(1), 2.0f);
    AssertHelper::assertFloatEquals(box->getHalfSize(2), 1.0f);
    AssertHelper::assertVector3FloatEquals(box->getNormalizedAxis(0), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::assertVector3FloatEquals(box->getNormalizedAxis(1), Vector3<float>(0.0f, 0.0f, 1.0f));
    AssertHelper::assertVector3FloatEquals(box->getNormalizedAxis(2), Vector3<float>(0.0f, -1.0f, 0.0f));
}

void ShapeToConvexObjectTest::capsuleConversion() {
    CollisionCapsuleShape collisionCapsule(0.5f, 5.0f, CapsuleShape<float>::CAPSULE_Y);
    PhysicsTransform transform(Point3<float>(1.0f, 1.0f, 1.0f), //move 1 unit on X, Y and Z axis
            Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f)); //rotate 90° on X axis

    auto capsuleObject = collisionCapsule.toConvexObject(transform);
    const auto* capsule = dynamic_cast<CollisionCapsuleObject*>(capsuleObject.get());

    AssertHelper::assertPoint3FloatEquals(capsule->getCenterOfMass(), Point3<float>(1.0f, 1.0f, 1.0f));
    AssertHelper::assertFloatEquals(capsule->getCylinderHeight(), 5.0f);
    AssertHelper::assertFloatEquals(capsule->getRadius(), 0.5f);
    AssertHelper::assertVector3FloatEquals(capsule->getAxis(0), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::assertVector3FloatEquals(capsule->getAxis(1), Vector3<float>(0.0f, 0.0f, 1.0f));
    AssertHelper::assertVector3FloatEquals(capsule->getAxis(2), Vector3<float>(0.0f, -1.0f, 0.0f));
}

void ShapeToConvexObjectTest::cylinderConversion() {
    CollisionCylinderShape collisionCylinder(0.5f, 5.0f, CylinderShape<float>::CYLINDER_Y);
    PhysicsTransform transform(Point3<float>(1.0f, 1.0f, 1.0f), //move 1 unit on X, Y and Z axis
            Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f)); //rotate 90° on X axis

    auto cylinderObject = collisionCylinder.toConvexObject(transform);
    const auto* cylinder = dynamic_cast<CollisionCylinderObject*>(cylinderObject.get());

    AssertHelper::assertPoint3FloatEquals(cylinder->getCenterOfMass(), Point3<float>(1.0f, 1.0f, 1.0f));
    AssertHelper::assertFloatEquals(cylinder->getHeight(), 5.0f);
    AssertHelper::assertFloatEquals(cylinder->getRadius(), 0.5f);
    AssertHelper::assertVector3FloatEquals(cylinder->getAxis(0), Vector3<float>(1.0f, 0.0f, 0.0f));
    AssertHelper::assertVector3FloatEquals(cylinder->getAxis(1), Vector3<float>(0.0f, 0.0f, 1.0f));
    AssertHelper::assertVector3FloatEquals(cylinder->getAxis(2), Vector3<float>(0.0f, -1.0f, 0.0f));
}

void ShapeToConvexObjectTest::convexHullConversion() {
    std::vector<Point3<float>> trapezePoints = {
            Point3<float>(1.0f, 0.0f, 0.0f), Point3<float>(3.0f, 0.0f, 0.0f), Point3<float>(0.0f, 3.0f, 0.0f), Point3<float>(0.0f, 1.0f, 0.0f),
            Point3<float>(1.0f, 0.0f, -1.0f), Point3<float>(3.0f, 0.0f, -1.0f), Point3<float>(0.0f, 3.0f, -1.0f), Point3<float>(0.0f, 1.0f, -1.0f),
    };

    CollisionConvexHullShape collisionConvexHull(trapezePoints);
    PhysicsTransform transform(Point3<float>(0.0f, 0.0f, 0.0f), //move 0 unit on X, Y and Z axis
            Quaternion<float>::rotationZ(-MathValue::PI_FLOAT / 4.0f)); //rotate 45° on Z axis

    auto convexHullObject = collisionConvexHull.toConvexObject(transform);
    const auto* convexHull = dynamic_cast<CollisionConvexHullObject*>(convexHullObject.get());

    AssertHelper::assertPoint3FloatEquals(convexHull->getConvexHullWithoutMargin().getPoints()[0], Point3<float>(0.74710678118f, -0.6905382872f, -0.04f));
    AssertHelper::assertPoint3FloatEquals(convexHull->getConvexHullWithoutMargin().getPoints()[1], Point3<float>(2.08132034356f, -2.0247519016f, -0.04f));
}

void ShapeToConvexObjectTest::sphereConversion() {
    CollisionSphereShape collisionSphere(20.0f); //sphere radius 20 units
    PhysicsTransform transform(Point3<float>(1.0f, 1.0f, 1.0f), //move 1 unit on X, Y and Z axis
            Quaternion<float>::rotationX(MathValue::PI_FLOAT / 2.0f)); //rotate 90° on X axis

    auto sphereObject = collisionSphere.toConvexObject(transform);
    const auto* sphere = dynamic_cast<CollisionSphereObject*>(sphereObject.get());

    AssertHelper::assertFloatEquals(sphere->getRadius(), 20.0f);
    AssertHelper::assertPoint3FloatEquals(sphere->getCenterOfMass(), Point3<float>(1.0f, 1.0f, 1.0f));
}

CppUnit::Test* ShapeToConvexObjectTest::suite() {
    auto* suite = new CppUnit::TestSuite("ShapeToConvexObjectTest");

    suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("boxConversion", &ShapeToConvexObjectTest::boxConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("capsuleConversion", &ShapeToConvexObjectTest::capsuleConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("cylinderConversion", &ShapeToConvexObjectTest::cylinderConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("convexHullConversion", &ShapeToConvexObjectTest::convexHullConversion));
    suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("sphereConversion", &ShapeToConvexObjectTest::sphereConversion));

    return suite;
}
