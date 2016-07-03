#include <queue>
#include "UrchinCommon.h"

#include "collision/broadphase/aabbtree/AABBTree.h"
#include "collision/broadphase/aabbtree/BodyNodeData.h"
#include "collision/broadphase/VectorPairContainer.h"

namespace urchin
{

	AABBTree::AABBTree() :
			fatMargin(ConfigService::instance()->getFloatValue("broadPhase.aabbTreeFatMargin")),
			rootNode(nullptr),
			defaultPairContainer(new VectorPairContainer())
	{

	}

	AABBTree::~AABBTree()
	{
		delete rootNode;
		delete defaultPairContainer;
	}

	void AABBTree::addBody(AbstractWorkBody *body, PairContainer *alternativePairContainer)
	{
		BodyNodeData *nodeData = new BodyNodeData(body, alternativePairContainer);
		AABBNode *nodeToInsert = new AABBNode(nodeData);

		if (rootNode!=nullptr)
		{
			nodeToInsert->updateAABBox(fatMargin);
			insertNode(nodeToInsert, rootNode);
		}else
		{
			rootNode = nodeToInsert;
			rootNode->updateAABBox(fatMargin);
		}

		bodiesNode[body] = nodeToInsert;

		computeOverlappingPairsFor(nodeToInsert, rootNode);
	}

	void AABBTree::insertNode(AABBNode *nodeToInsert, AABBNode *currentNode)
	{
		if (currentNode->isLeaf())
		{
			AABBNode *newParent = new AABBNode(nullptr);
			newParent->setParent(currentNode->getParent());
			newParent->setLeftChild(nodeToInsert);
			newParent->setRightChild(currentNode);
			newParent->updateAABBox(fatMargin);
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

			currentNode->updateAABBox(fatMargin);
		}
	}

	void AABBTree::computeOverlappingPairsFor(AABBNode *leafNode, AABBNode *rootNode)
	{
		std::queue<AABBNode *> stackNodes;
		stackNodes.push(rootNode);

		while(!stackNodes.empty())
		{
			AABBNode *currentNode = stackNodes.front();
			stackNodes.pop();

			if(leafNode->getAABBox().collideWithAABBox(currentNode->getAABBox()))
			{
				if (currentNode->isLeaf())
				{
					createOverlappingPair(leafNode->getBodyNodeData(), currentNode->getBodyNodeData());
				}else
				{
					stackNodes.push(currentNode->getLeftChild());
					stackNodes.push(currentNode->getRightChild());
				}
			}
		}
	}

	void AABBTree::removeBody(AbstractWorkBody *body)
	{
		std::map<AbstractWorkBody *, AABBNode *>::iterator it = bodiesNode.find(body);

		AABBNode *nodeToRemove = it->second;
		AABBNode *parentNode = nodeToRemove->getParent();

		if(parentNode==nullptr)
		{
			rootNode = nullptr;
		}else
		{
			AABBNode *sibling = nodeToRemove->getSibling();
			if(parentNode->getParent()!=nullptr)
			{
				if(parentNode->getParent()->getLeftChild()==parentNode)
				{
					parentNode->getParent()->setLeftChild(sibling);
				}else
				{
					parentNode->getParent()->setRightChild(sibling);
				}
			}else
			{
				rootNode = sibling;
				sibling->setParent(nullptr);
			}

		    delete parentNode;
		}

		removeOverlappingPairs(nodeToRemove->getBodyNodeData());

		bodiesNode.erase(it);
		delete nodeToRemove;
	}

	void AABBTree::updateBodies()
	{
		for(std::map<AbstractWorkBody *, AABBNode *>::iterator it = bodiesNode.begin(); it!=bodiesNode.end(); ++it)
		{
			AbstractWorkBody *body = it->first;
			if(body->isActive())
			{
				AABBNode *leaf = it->second;
				const AABBox<float> &leafFatAABBox = leaf->getAABBox();
				const AABBox<float> &bodyAABBox = leaf->getBodyNodeData()->retrieveBodyAABBox();
				if(!leafFatAABBox.include(bodyAABBox))
				{
					removeBody(body);
					addBody(body, leaf->getBodyNodeData()->getAlternativePairContainer());
				}
			}
		}
	}

	const std::vector<OverlappingPair *> &AABBTree::getOverlappingPairs() const
	{
		return defaultPairContainer->getOverlappingPairs();
	}

	void AABBTree::createOverlappingPair(BodyNodeData *nodeData1, BodyNodeData *nodeData2)
	{
		if(!nodeData1->hasAlternativePairContainer() && !nodeData2->hasAlternativePairContainer())
		{
			defaultPairContainer->addOverlappingPair(nodeData1->getBody(), nodeData2->getBody());
		}else
		{
			if(nodeData1->hasAlternativePairContainer())
			{
				nodeData1->getAlternativePairContainer()->addOverlappingPair(nodeData1->getBody(), nodeData2->getBody());
				nodeData2->addOwnerPairContainer(nodeData1->getAlternativePairContainer());
			}

			if(nodeData2->hasAlternativePairContainer())
			{
				nodeData2->getAlternativePairContainer()->addOverlappingPair(nodeData1->getBody(), nodeData2->getBody());
				nodeData1->addOwnerPairContainer(nodeData2->getAlternativePairContainer());
			}
		}
	}

	void AABBTree::removeOverlappingPairs(const BodyNodeData *nodeData)
	{
		if(!nodeData->hasAlternativePairContainer())
		{
			defaultPairContainer->removeOverlappingPairs(nodeData->getBody());
		}else
		{
			nodeData->getAlternativePairContainer()->removeOverlappingPairs(nodeData->getBody());
		}

		for(auto &ownerPairContainer : nodeData->getOwnerPairContainers())
		{
			ownerPairContainer->removeOverlappingPairs(nodeData->getBody());
		}
	}

}
