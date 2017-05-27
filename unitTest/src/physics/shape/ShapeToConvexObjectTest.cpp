#include <cppunit/TestSuite.h>
#include <cppunit/TestCaller.h>
#include <memory>
#include "UrchinCommon.h"
#include "UrchinPhysicsEngine.h"

#include "AssertHelper.h"
#include "physics/shape/ShapeToConvexObjectTest.h"
using namespace urchin;

void ShapeToConvexObjectTest::boxConversion()
{
	CollisionBoxShape collisionBox(Vector3<float>(1.0, 2.0, 1.0)); //box 1x2x1
	PhysicsTransform transform(urchin::Point3<float>(1.0, 1.0, 1.0), //move 1 unit on X, Y and Z axis
			urchin::Quaternion<float>(urchin::Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2)); //rotate 90° on X axis

	std::shared_ptr<CollisionConvexObject3D> boxObject = collisionBox.toConvexObject(transform);
	CollisionBoxObject *box = static_cast<CollisionBoxObject *>(boxObject.get());

	AssertHelper::assertPoint3FloatEquals(box->getCenterOfMass(), Point3<float>(1.0, 1.0, 1.0));
	AssertHelper::assertFloatEquals(box->getHalfSize(0), 1.0);
	AssertHelper::assertFloatEquals(box->getHalfSize(1), 2.0);
	AssertHelper::assertFloatEquals(box->getHalfSize(2), 1.0);
	AssertHelper::assertVector3FloatEquals(box->getAxis(0), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertVector3FloatEquals(box->getAxis(1), Vector3<float>(0.0, 0.0, 1.0));
	AssertHelper::assertVector3FloatEquals(box->getAxis(2), Vector3<float>(0.0, -1.0, 0.0));
}

void ShapeToConvexObjectTest::capsuleConversion()
{
	CollisionCapsuleShape collisionCapsule(0.5, 5.0, CapsuleShape<float>::CAPSULE_Y);
	PhysicsTransform transform(urchin::Point3<float>(1.0, 1.0, 1.0), //move 1 unit on X, Y and Z axis
			urchin::Quaternion<float>(urchin::Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2)); //rotate 90° on X axis

	std::shared_ptr<CollisionConvexObject3D> capsuleObject = collisionCapsule.toConvexObject(transform);
	CollisionCapsuleObject *capsule = static_cast<CollisionCapsuleObject *>(capsuleObject.get());

	AssertHelper::assertPoint3FloatEquals(capsule->getCenterOfMass(), Point3<float>(1.0, 1.0, 1.0));
	AssertHelper::assertFloatEquals(capsule->getCylinderHeight(), 5.0);
	AssertHelper::assertFloatEquals(capsule->getRadius(), 0.5);
	AssertHelper::assertVector3FloatEquals(capsule->getAxis(0), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertVector3FloatEquals(capsule->getAxis(1), Vector3<float>(0.0, 0.0, 1.0));
	AssertHelper::assertVector3FloatEquals(capsule->getAxis(2), Vector3<float>(0.0, -1.0, 0.0));
}

void ShapeToConvexObjectTest::cylinderConversion()
{
	CollisionCylinderShape collisionCylinder(0.5, 5.0, CylinderShape<float>::CYLINDER_Y);
	PhysicsTransform transform(urchin::Point3<float>(1.0, 1.0, 1.0), //move 1 unit on X, Y and Z axis
			urchin::Quaternion<float>(urchin::Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2)); //rotate 90° on X axis

	std::shared_ptr<CollisionConvexObject3D> cylinderObject = collisionCylinder.toConvexObject(transform);
	CollisionCylinderObject *cylinder = static_cast<CollisionCylinderObject *>(cylinderObject.get());

	AssertHelper::assertPoint3FloatEquals(cylinder->getCenterOfMass(), Point3<float>(1.0, 1.0, 1.0));
	AssertHelper::assertFloatEquals(cylinder->getHeight(), 5.0);
	AssertHelper::assertFloatEquals(cylinder->getRadius(), 0.5);
	AssertHelper::assertVector3FloatEquals(cylinder->getAxis(0), Vector3<float>(1.0, 0.0, 0.0));
	AssertHelper::assertVector3FloatEquals(cylinder->getAxis(1), Vector3<float>(0.0, 0.0, 1.0));
	AssertHelper::assertVector3FloatEquals(cylinder->getAxis(2), Vector3<float>(0.0, -1.0, 0.0));
}

void ShapeToConvexObjectTest::convexHullConversion()
{
	Point3<float> trapezePointsTab[] = {
			Point3<float>(1.0, 0.0, 0.0), Point3<float>(3.0, 0.0, 0.0), Point3<float>(0.0, 3.0, 0.0), Point3<float>(0.0, 1.0, 0.0),
			Point3<float>(1.0, 0.0, -1.0), Point3<float>(3.0, 0.0, -1.0), Point3<float>(0.0, 3.0, -1.0), Point3<float>(0.0, 1.0, -1.0),
	};
	std::vector<Point3<float>> trapezePoints(trapezePointsTab, trapezePointsTab+sizeof(trapezePointsTab)/sizeof(Point3<float>));

	CollisionConvexHullShape collisionConvexHull(trapezePoints);
	PhysicsTransform transform(urchin::Point3<float>(0.0, 0.0, 0.0), //move 0 unit on X, Y and Z axis
			urchin::Quaternion<float>(urchin::Vector3<float>(0.0, 0.0, 1.0), -PI_VALUE/4)); //rotate 45° on Z axis

	std::shared_ptr<CollisionConvexObject3D> convexHullObject = collisionConvexHull.toConvexObject(transform);
	CollisionConvexHullObject *convexHull = static_cast<CollisionConvexHullObject *>(convexHullObject.get());

	AssertHelper::assertPoint3FloatEquals(convexHull->getPointsWithoutMargin()[0], Point3<float>(0.70710678118, -0.70710678118, 0.0));
	AssertHelper::assertPoint3FloatEquals(convexHull->getPointsWithoutMargin()[1], Point3<float>(2.12132034356, -2.12132034356, 0.0));
}

void ShapeToConvexObjectTest::sphereConversion()
{
	CollisionSphereShape collisionSphere(20.0); //sphere radius 20 units
	PhysicsTransform transform(urchin::Point3<float>(1.0, 1.0, 1.0), //move 1 unit on X, Y and Z axis
			urchin::Quaternion<float>(urchin::Vector3<float>(1.0, 0.0, 0.0), PI_VALUE/2)); //rotate 90° on X axis

	std::shared_ptr<CollisionConvexObject3D> sphereObject = collisionSphere.toConvexObject(transform);
	CollisionSphereObject *sphere = static_cast<CollisionSphereObject *>(sphereObject.get());

	AssertHelper::assertFloatEquals(sphere->getRadius(), 20.0);
	AssertHelper::assertPoint3FloatEquals(sphere->getCenterOfMass(), Point3<float>(1.0, 1.0, 1.0));
}

CppUnit::Test *ShapeToConvexObjectTest::suite()
{
	CppUnit::TestSuite *suite = new CppUnit::TestSuite("ShapeToConvexObjectTest");

	suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("boxConversion", &ShapeToConvexObjectTest::boxConversion));
	suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("capsuleConversion", &ShapeToConvexObjectTest::capsuleConversion));
	suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("cylinderConversion", &ShapeToConvexObjectTest::cylinderConversion));
	suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("convexHullConversion", &ShapeToConvexObjectTest::convexHullConversion));
	suite->addTest(new CppUnit::TestCaller<ShapeToConvexObjectTest>("sphereConversion", &ShapeToConvexObjectTest::sphereConversion));

	return suite;
}
