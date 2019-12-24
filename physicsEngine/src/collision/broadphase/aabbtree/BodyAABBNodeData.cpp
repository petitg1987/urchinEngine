#include "collision/broadphase/aabbtree/BodyAABBNodeData.h"

namespace urchin
{

	BodyAABBNodeData::BodyAABBNodeData(AbstractWorkBody *body, PairContainer *alternativePairContainer) :
		AABBNodeData(body),
		alternativePairContainer(alternativePairContainer)
	{

	}

    AABBNodeData<AbstractWorkBody *> *BodyAABBNodeData::clone() const
    {
	    return new BodyAABBNodeData(getNodeObject(), alternativePairContainer);
    }

    const std::string &BodyAABBNodeData::getObjectId() const
    {
	    return getNodeObject()->getId();
    }

	AABBox<float> BodyAABBNodeData::retrieveObjectAABBox() const
	{
		return getNodeObject()->getShape()->toAABBox(getNodeObject()->getPhysicsTransform());
	}

    bool BodyAABBNodeData::isObjectMoving() const
    {
	    return getNodeObject()->isActive();
    }

	bool BodyAABBNodeData::hasAlternativePairContainer() const
	{
		return alternativePairContainer != nullptr;
	}

	PairContainer *BodyAABBNodeData::getAlternativePairContainer() const
	{
		return alternativePairContainer;
	}

	void BodyAABBNodeData::addOwnerPairContainer(PairContainer *ownerPairContainer)
	{
		ownerPairContainers.insert(ownerPairContainer);
	}

    void BodyAABBNodeData::removeOwnerPairContainer(PairContainer *ownerPairContainer)
    {
        ownerPairContainers.erase(ownerPairContainer);
    }

	/**
	 * Returns pair containers which have pair(s) with this node data
	 */
	std::set<PairContainer *> BodyAABBNodeData::getOwnerPairContainers() const
	{
		return ownerPairContainers;
	}

}
