#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"
#include "processable/raytest/RayTester.h"

namespace urchin
{

	RayTester::RayTester(const Ray<float> &ray) :
			ray(ray),
			rayTestResult(std::make_shared<RayTestResult>()),
			collisionWorld(nullptr)
	{ //TODO don't work on testEngineSfml

	}

	std::shared_ptr<const RayTestResult> RayTester::getRayTestResult() const
	{
		return rayTestResult;
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
		ccd_set rayCastResults = collisionWorld->getNarrowPhaseManager()->rayTest(ray, bodiesAABBoxHitRay);

		rayTestResult->addResults(rayCastResults);
	}

}
