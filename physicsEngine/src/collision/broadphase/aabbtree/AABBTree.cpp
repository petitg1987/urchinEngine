#include "collision/broadphase/aabbtree/AABBTree.h"
#include "collision/broadphase/aabbtree/BodyNodeData.h"

namespace urchin
{

	AABBTree::AABBTree() :
			rootNode(nullptr)
	{

	}

	AABBTree::~AABBTree()
	{
		delete rootNode;
	}

	void AABBTree::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{
		BodyNodeData *nodeData = new BodyNodeData(body, alternativePairContainer);
		AABBNode *node = new AABBNode(nodeData);

		if (rootNode!=nullptr)
		{
			node->updateAABBox();
			insertNode(node, rootNode);
		}else
		{
			rootNode = node;
			rootNode->updateAABBox();
		}

		bodiesNode[body] = node;
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

	void AABBTree::insertNode(AABBNode *nodeToInsert, AABBNode *currentNode)
	{
		if (currentNode->isLeaf())
		{
			AABBNode *newParent = new AABBNode(nullptr);
			newParent->setParent(currentNode->getParent());
			newParent->setChildren(nodeToInsert, currentNode);
			newParent->updateAABBox();
		}else
		{
			const AABBox<float> &leftAABBox = currentNode->getLeftChild()->getAABBox();
			float volumeDiffLeft = leftAABBox.merge(nodeToInsert->getAABBox()).getVolume() - leftAABBox.getVolume();

			const AABBox<float> &rightAABBox = currentNode->getRightChild()->getAABBox();
			float volumeDiffRight = rightAABBox.merge(nodeToInsert->getAABBox()).getVolume() - rightAABBox.getVolume();

			if(volumeDiffLeft < volumeDiffRight)
			{
				insertNode(nodeToInsert, currentNode->getLeftChild());
			}else
			{
				insertNode(nodeToInsert, currentNode->getRightChild());
			}

			currentNode->updateAABBox();
		}
	}

}
