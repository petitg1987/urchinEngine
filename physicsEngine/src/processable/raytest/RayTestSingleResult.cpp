#include "processable/raytest/RayTestSingleResult.h"
#include "body/AbstractBody.h"

namespace urchin
{

	RayTestSingleResult::RayTestSingleResult(const Vector3<float> &normal, const Point3<float> &hitPointB, float timeToHit) :
		normal(normal),
		hitPointB(hitPointB),
		timeToHit(timeToHit)
	{

	}

	RayTestSingleResult::~RayTestSingleResult()
	{

	}

	const Vector3<float> &RayTestSingleResult::getNormal() const
	{
		return normal;
	}

	const Point3<float> &RayTestSingleResult::getHitPointB() const
	{
		return hitPointB;
	}

	/**
	 * Return time to hit. A value of 0.0 means objects collide at ray origin.
	 * A value of 1.0 means objects will collide at ray target.
	 */
	float RayTestSingleResult::getTimeToHit() const
	{
		return timeToHit;
	}

}
