#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"

namespace urchin
{

	CollisionAlgorithm::CollisionAlgorithm(bool objectSwapped, const ManifoldResult &manifoldResult) :
			objectSwapped(objectSwapped),
			manifoldResult(manifoldResult),
            collisionAlgorithmSelector(nullptr)
	{

	}

    void CollisionAlgorithm::setupCollisionAlgorithmSelector(const CollisionAlgorithmSelector *collisionAlgorithmSelector)
    {
	    this->collisionAlgorithmSelector = collisionAlgorithmSelector;
    }

	void CollisionAlgorithm::processCollisionAlgorithm(const CollisionObjectWrapper &object1, const CollisionObjectWrapper &object2, bool refreshContractPoints)
	{
		if(objectSwapped)
		{
			doProcessCollisionAlgorithm(object2, object1);
		}else
		{
			doProcessCollisionAlgorithm(object1, object2);
		}

		if(refreshContractPoints)
		{
			refreshContactPoints();
		}
	}

	const ManifoldResult &CollisionAlgorithm::getConstManifoldResult() const
	{
		return manifoldResult;
	}

	bool CollisionAlgorithm::isObjectSwapped() const
	{
		return objectSwapped;
	}

	const CollisionAlgorithmSelector *CollisionAlgorithm::getCollisionAlgorithmSelector() const
	{
		return collisionAlgorithmSelector;
	}

	ManifoldResult &CollisionAlgorithm::getManifoldResult()
	{
		return manifoldResult;
	}

	/**
	 * @param normalFromObject2 Contact normal from object 2. The normal direction should be toward the object 1.
	 * @param depth Penetration depth (negative when collision exist)
	 */
	void CollisionAlgorithm::addNewContactPoint(const Vector3<float> &normalFromObject2, const Point3<float> &pointOnObject2, float depth)
	{
		manifoldResult.addContactPoint(normalFromObject2, pointOnObject2, depth, false);
	}

	void CollisionAlgorithm::refreshContactPoints()
	{
		ScopeProfiler profiler("physics", "reContactPts");

		manifoldResult.refreshContactPoints();
	}

	float CollisionAlgorithm::getContactBreakingThreshold() const
	{
		return manifoldResult.getContactBreakingThreshold();
	}
}
