#include "collision/broadphase/aabbtree/BodyNodeData.h"
#include "collision/broadphase/aabbtree/AABBTree.h"

namespace urchin
{

	BodyNodeData::BodyNodeData(AbstractWorkBody *body, PairContainer *alternativePairContainer) :
		body(body),
		alternativePairContainer(alternativePairContainer)
	{

	}

	BodyNodeData::~BodyNodeData()
	{

	}

	AbstractWorkBody *BodyNodeData::getBody() const
	{
		return body;
	}

	AABBox<float> BodyNodeData::retrieveBodyAABBox() const
	{
		return body->getShape()->toAABBox(body->getPhysicsTransform());
	}

	bool BodyNodeData::hasAlternativePairContainer() const
	{
		return alternativePairContainer != nullptr;
	}

	PairContainer *BodyNodeData::getAlternativePairContainer() const
	{
		return alternativePairContainer;
	}

	void BodyNodeData::addOwnerPairContainer(PairContainer *ownerPairContainer)
	{
		ownerPairContainers.insert(ownerPairContainer);
	}

	/**
	 * Returns pair containers which have pair(s) with this node data
	 */
	std::set<PairContainer *> BodyNodeData::getOwnerPairContainers() const
	{
		return ownerPairContainers;
	}

}
