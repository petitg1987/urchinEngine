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

}
