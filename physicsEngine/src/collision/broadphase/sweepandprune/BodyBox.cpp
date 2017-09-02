#include "collision/broadphase/sweepandprune/BodyBox.h"

namespace urchin
{

	/**
	* @param alternativePairContainer Alternative pair container to default one
	*/
	BodyBox::BodyBox(AbstractWorkBody *body, PairContainer *alternativePairContainer) :
		body(body),
		alternativePairContainer(alternativePairContainer)
	{

	}

	AbstractWorkBody *BodyBox::getBody() const
	{
		return body;
	}

	AABBox<float> BodyBox::retrieveBodyAABBox() const
	{
		return body->getShape()->toAABBox(body->getPhysicsTransform());
	}

	bool BodyBox::hasAlternativePairContainer() const
	{
		return alternativePairContainer != nullptr;
	}

	PairContainer *BodyBox::getAlternativePairContainer() const
	{
		return alternativePairContainer;
	}

}
