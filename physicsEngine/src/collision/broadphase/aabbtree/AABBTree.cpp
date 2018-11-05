#include <stack>
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
		auto *nodeData = new BodyNodeData(body, alternativePairContainer);
		auto *nodeToInsert = new AABBNode(nodeData);

		if (rootNode)
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

		#ifdef _DEBUG
			//printTree(rootNode, 0);
		#endif
	}

	void AABBTree::insertNode(AABBNode *nodeToInsert, AABBNode *currentNode)
	{
		if (currentNode->isLeaf())
		{
			auto *newParent = new AABBNode(nullptr);
			replaceNode(currentNode, newParent);
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

	void AABBTree::replaceNode(AABBNode *nodeToReplace, AABBNode *newNode)
	{
		if(nodeToReplace->getParent())
		{
			if(nodeToReplace->getParent()->getLeftChild()==nodeToReplace)
			{
				nodeToReplace->getParent()->setLeftChild(newNode);
			}else
			{
				nodeToReplace->getParent()->setRightChild(newNode);
			}
		}else
		{
			rootNode = newNode;
			newNode->setParent(nullptr);
		}
	}

	void AABBTree::computeOverlappingPairsFor(AABBNode *leafNode, AABBNode *rootNode)
	{
		std::stack<AABBNode *> stackNodes;
		stackNodes.push(rootNode);

		while(!stackNodes.empty())
		{ //tree traversal: pre-order (iterative)
			AABBNode *currentNode = stackNodes.top();
			stackNodes.pop();

			if(leafNode!=currentNode && leafNode->getAABBox().collideWithAABBox(currentNode->getAABBox()))
			{
				if (currentNode->isLeaf())
				{
					createOverlappingPair(leafNode->getBodyNodeData(), currentNode->getBodyNodeData());
				}else
				{
					stackNodes.push(currentNode->getRightChild());
					stackNodes.push(currentNode->getLeftChild());
				}
			}
		}
	}

	void AABBTree::removeBody(AbstractWorkBody *body)
	{
		auto it = bodiesNode.find(body);

		removeNode(it->second);
		bodiesNode.erase(it);
	}

	void AABBTree::removeNode(AABBNode *nodeToRemove)
	{
		AABBNode *parentNode = nodeToRemove->getParent();

		if(!parentNode)
		{
			rootNode = nullptr;
		}else
		{
			AABBNode *sibling = nodeToRemove->getSibling();
			replaceNode(parentNode, sibling);

			parentNode->setLeftChild(nullptr); //avoid child removal
			parentNode->setRightChild(nullptr); //avoid child removal
		    delete parentNode;
		}

		removeOverlappingPairs(nodeToRemove->getBodyNodeData());

		delete nodeToRemove;
	}

	void AABBTree::updateBodies()
	{
		for(auto it = bodiesNode.begin(); it!=bodiesNode.end();)
		{
			bool removed = false;
			AbstractWorkBody *body = it->first;
			if(body->isActive())
			{
				AABBNode *leaf = it->second;
				const AABBox<float> &leafFatAABBox = leaf->getAABBox();
				const AABBox<float> &bodyAABBox = leaf->getBodyNodeData()->retrieveBodyAABBox();
				if(!leafFatAABBox.include(bodyAABBox))
				{
					PairContainer *alternativePairContainer = leaf->getBodyNodeData()->getAlternativePairContainer();

					removeNode(leaf);
					it = bodiesNode.erase(it);
					removed = true;

					addBody(body, alternativePairContainer);
				}
			}

			if(!removed)
			{
				++it;
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

	/**
	 * @param bodiesAABBoxHitRay [out] Bodies AABBox hit by the ray
	 */
	void AABBTree::rayTest(const Ray<float> &ray, std::vector<AbstractWorkBody *> &bodiesAABBoxHitRay) const
	{
		enlargedRayTest(ray, 0.0f, nullptr, bodiesAABBoxHitRay);
	}

	/**
	 * Enlarge each node box of a specified size and process a classical ray test. This method provide similar result to a OBB test but with better performance.
	 * @param enlargeNodeBoxHalfSize Specify the size of the enlargement. A size of 0.5 will enlarge the node box from 1.0 (0.5 on left and 0.5 on right).
	 * @param bodiesAABBoxHitEnlargedRay [out] Bodies AABBox hit by the enlarged ray
	 */
	void AABBTree::enlargedRayTest(const Ray<float> &ray, float enlargeNodeBoxHalfSize, const AbstractWorkBody *testedBody,
			std::vector<AbstractWorkBody *> &bodiesAABBoxHitEnlargedRay) const
	{
		std::stack<AABBNode *> stackNodes;
		stackNodes.push(rootNode);

		while(!stackNodes.empty())
		{ //tree traversal: pre-order (iterative)
			AABBNode *currentNode = stackNodes.top();
			stackNodes.pop();

			AABBox<float> extendedNodeAABBox = currentNode->getAABBox().enlarge(enlargeNodeBoxHalfSize, enlargeNodeBoxHalfSize);
			if(extendedNodeAABBox.collideWithRay(ray))
			{
				if (currentNode->isLeaf())
				{
					AbstractWorkBody *body = currentNode->getBodyNodeData()->getBody();
					if(body!=testedBody)
					{
						bodiesAABBoxHitEnlargedRay.push_back(body);
					}
				}else
				{
					stackNodes.push(currentNode->getRightChild());
					stackNodes.push(currentNode->getLeftChild());
				}
			}
		}
	}

#ifdef _DEBUG
	void AABBTree::printTree(AABBNode *node, unsigned int indentLevel)
	{ //tree traversal: pre-order (recursive)
		if(node->isLeaf())
		{
			std::cout<<std::string(indentLevel, ' ')<<"- Leaf: "<<node->getBodyNodeData()->getBody()->getId()<<std::endl;
		}else
		{
			if(!node->getParent())
			{
				std::cout<<std::string(indentLevel, ' ')<<"Root:"<<std::endl;
			}else
			{
				std::cout<<std::string(indentLevel, ' ')<<"- Node:"<<std::endl;
			}

			printTree(node->getLeftChild(), indentLevel + 2);
			printTree(node->getRightChild(), indentLevel + 2);
		}

		if(indentLevel==0)
		{
			std::cout<<std::endl;
		}
	}
#endif

}
