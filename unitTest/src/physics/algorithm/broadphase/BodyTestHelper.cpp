#include <memory>

#include "physics/algorithm/broadphase/BodyTestHelper.h"
using namespace urchin;

std::unique_ptr<WorkRigidBody> BodyTestHelper::createCubeRigidBody(const Point3<float> &position, float boxSize)
{
	float halfBoxSize = boxSize / 2.0f;

	std::shared_ptr<CollisionBoxShape> boxBody = std::make_shared<CollisionBoxShape>(Vector3<float>(halfBoxSize, halfBoxSize, halfBoxSize));
	std::unique_ptr<WorkRigidBody> rigidBody = std::make_unique<WorkRigidBody>("bodyName", PhysicsTransform(position, Quaternion<float>()), boxBody);
	rigidBody->setIsStatic(false);
	rigidBody->setTotalMomentum(Vector3<float>(1.0, 1.0, 1.0));

	return rigidBody;
}
