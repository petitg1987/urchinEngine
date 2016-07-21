#include "processable/raytest/RayTester.h"
#include "PhysicsWorld.h"

namespace urchin
{

	RayTester::RayTester(const Ray<float> &ray, RayTestCallback &rayTestCallback) :
			ray(ray),
			rayTestCallback(rayTestCallback),
			collisionWorld(nullptr)
	{

	}

	RayTester::~RayTester()
	{

	}

	void RayTester::initialize(PhysicsWorld *physicsWorld)
	{
		collisionWorld = physicsWorld->getCollisionWorld();
	}

	void RayTester::setup(float, const Vector3<float> &)
	{
		//nothing to do
	}

	void RayTester::execute(float, const Vector3<float> &)
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitRay = collisionWorld->getBroadPhaseManager()->rayTest(ray);

		collisionWorld->getNarrowPhaseManager()->rayTest(ray, bodiesAABBoxHitRay, rayTestCallback);
	}

}
