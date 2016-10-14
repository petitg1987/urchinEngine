#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"

namespace urchin
{

	AABBTreeAlgorithm::AABBTreeAlgorithm() :
			tree(new AABBTree())
	{

	}

	AABBTreeAlgorithm::~AABBTreeAlgorithm()
	{
		delete tree;
	}

	void AABBTreeAlgorithm::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{
		tree->addBody(body, alternativePairContainer);
	}

	void AABBTreeAlgorithm::removeBody(AbstractWorkBody *body)
	{
		tree->removeBody(body);
	}

	void AABBTreeAlgorithm::updateBodies()
	{
		tree->updateBodies();
	}

	const std::vector<OverlappingPair *> &AABBTreeAlgorithm::getOverlappingPairs() const
	{
		return tree->getOverlappingPairs();
	}

	std::vector<AbstractWorkBody *> AABBTreeAlgorithm::rayTest(const Ray<float> &ray) const
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitRay;
		bodiesAABBoxHitRay.reserve(15);

		tree->rayTest(ray, bodiesAABBoxHitRay);

		return bodiesAABBoxHitRay;
	}

	/**
	 * Enlarge each node box of a specified size and process a classical ray test. This method provide similar result to a OBB test but with better performance.
	 * @param enlargeNodeBoxHalfSize Specify the size of the enlargement. A size of 0.5 in X axis will enlarge the node box from 1.0 (0.5 on left and 0.5 on right).
	 */
	std::vector<AbstractWorkBody *> AABBTreeAlgorithm::enlargedRayTest(const Ray<float> &ray, const Vector3<float> &enlargeNodeBoxHalfSize) const
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitEnlargedRay;
		bodiesAABBoxHitEnlargedRay.reserve(20);

		tree->enlargedRayTest(ray, enlargeNodeBoxHalfSize, bodiesAABBoxHitEnlargedRay);

		return bodiesAABBoxHitEnlargedRay;
	}

}
