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

}
