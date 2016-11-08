#include "collision/broadphase/aabbtree/AABBTreeAlgorithm.h"
#include "shape/CollisionSphereShape.h"

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
		bodiesAABBoxHitRay.reserve(10);

		tree->rayTest(ray, bodiesAABBoxHitRay);

		return bodiesAABBoxHitRay;
	}

	std::vector<AbstractWorkBody *> AABBTreeAlgorithm::bodyTest(const AbstractWorkBody *body, const PhysicsTransform &from, const PhysicsTransform &to) const
	{
		std::vector<AbstractWorkBody *> bodiesAABBoxHitBody;
		bodiesAABBoxHitBody.reserve(15);

		Ray<float> ray(from.getPosition(), to.getPosition());
		float bodyConfinedSphereRadius = body->getShape()->toConfinedSphereShape()->getRadius(); //TODO is it correct to use confined sphere ?

		tree->enlargedRayTest(ray, bodyConfinedSphereRadius, body, bodiesAABBoxHitBody);

		return bodiesAABBoxHitBody;
	}

}
