#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"
#include "processable/raytest/RayTester.h"
#include "PhysicsWorld.h"

namespace urchin
{

	RayTester::RayTester(const Ray<float> &ray) :
			ray(ray),
			rayTestResult(std::make_shared<RayTestResult>()),
			collisionWorld(nullptr)
	{

	}

	RayTester::~RayTester()
	{

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
		std::vector<std::shared_ptr<ContinuousCollisionResult<double>>> rayCastResults = collisionWorld->getNarrowPhaseManager()->rayTest(ray, bodiesAABBoxHitRay);

		std::vector<RayTestSingleResult> rayTestResults;
		rayTestResults.reserve(rayCastResults.size());

		for(const std::shared_ptr<ContinuousCollisionResult<double>> &rayCastResult : rayCastResults)
		{
			if(rayCastResult->hasTimeOfImpactResult())
			{
				RayTestSingleResult rayTestSingleResult(
						rayCastResult->getNormal().template cast<float>(),
						rayCastResult->getHitPointB().template cast<float>(),
						static_cast<float>(rayCastResult->getTimeToHit()));
				rayTestResults.push_back(rayTestSingleResult);
			}
		}

		rayTestResult->addResults(rayTestResults);
	}

}
