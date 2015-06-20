#include "collision/broadphase/BodyBox.h"

namespace urchin
{

	/**
	* @param alternativePairContainer Alternative pair container to default one
	*/
	BodyBox::BodyBox(AbstractWorkBody *owner, PairContainer *alternativePairContainer) :
		owner(owner),
		alternativePairContainer(alternativePairContainer)
	{

	}

	BodyBox::~BodyBox()
	{

	}

	AbstractWorkBody *BodyBox::getOwner() const
	{
		return owner;
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
