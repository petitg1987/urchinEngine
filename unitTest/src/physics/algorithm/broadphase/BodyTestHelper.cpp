#include <memory>

#include "physics/algorithm/broadphase/BodyTestHelper.h"
using namespace urchin;

BodyTestHelper::BodyTestHelper()
{

}

BodyTestHelper::~BodyTestHelper()
{

}

WorkRigidBody *BodyTestHelper::createCubeRigidBody(const Point3<float> &position, float boxSize)
{
	float halfBoxSize = boxSize / 2.0f;

	std::shared_ptr<CollisionBoxShape> boxBody = std::make_shared<CollisionBoxShape>(Vector3<float>(halfBoxSize, halfBoxSize, halfBoxSize));
	WorkRigidBody *rigidBody = new WorkRigidBody("bodyName", PhysicsTransform(position, Quaternion<float>()), boxBody);
	rigidBody->setIsStatic(false);
	rigidBody->setTotalForce(Vector3<float>(1.0, 1.0, 1.0));

	return rigidBody;
}
