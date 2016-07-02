#include "collision/broadphase/aabbtree/AABBTree.h"

namespace urchin
{

	AABBTree::AABBTree() :
			rootNode(new AABBNode())
	{

	}

	AABBTree::~AABBTree()
	{
		delete rootNode;
	}

	void AABBTree::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{

	}

	void AABBTree::removeBody(AbstractWorkBody *body)
	{

	}

	void AABBTree::updateBodies()
	{

	}

	const std::vector<OverlappingPair *> &AABBTree::getOverlappingPairs() const
	{
		return std::vector<OverlappingPair *>();
	}

}
