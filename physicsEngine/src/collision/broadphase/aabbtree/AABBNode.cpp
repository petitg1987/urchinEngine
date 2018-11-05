#include "collision/broadphase/aabbtree/AABBNode.h"

namespace urchin
{

	AABBNode::AABBNode(BodyNodeData *bodyNodeData) :
		bodyNodeData(bodyNodeData),
		parentNode(nullptr)
	{
		this->children[0] = nullptr;
		this->children[1] = nullptr;
	}

	AABBNode::~AABBNode()
	{
		delete children[0];
		delete children[1];

		delete bodyNodeData;
	}

	BodyNodeData *AABBNode::getBodyNodeData() const
	{
		return bodyNodeData;
	}

	bool AABBNode::isLeaf() const
	{
		return children[0]==nullptr;
	}

	bool AABBNode::isRoot() const
	{
		return parentNode==nullptr;
	}

	void AABBNode::setParent(AABBNode *parentNode)
	{
		this->parentNode = parentNode;
	}

	AABBNode *AABBNode::getParent() const
	{
		return parentNode;
	}

	void AABBNode::setLeftChild(AABBNode *leftChild)
	{
		this->children[0] = leftChild;
		if(leftChild)
		{
			this->children[0]->setParent(this);
		}
	}

	AABBNode *AABBNode::getLeftChild() const
	{
		return children[0];
	}

	void AABBNode::setRightChild(AABBNode *rightChild)
	{
		this->children[1] = rightChild;
		if(rightChild)
		{
			this->children[1]->setParent(this);
		}
	}

	AABBNode *AABBNode::getRightChild() const
	{
		return children[1];
	}

	AABBNode *AABBNode::getSibling() const
	{
		return parentNode->getLeftChild()==this ? parentNode->getRightChild() : parentNode->getLeftChild();
	}

	/**
	 * Returns fat AABBox for leaf and bounding box for branch
	 */
	const AABBox<float> &AABBNode::getAABBox() const
	{
		return aabbox;
	}

	void AABBNode::updateAABBox(float fatMargin)
	{
		if (isLeaf())
		{
			Point3<float> fatMargin3(fatMargin, fatMargin, fatMargin);
			AABBox<float> bodyBox = bodyNodeData->retrieveBodyAABBox();

			aabbox = AABBox<float>(bodyBox.getMin()-fatMargin3, bodyBox.getMax()+fatMargin3);
		}else
		{
			aabbox = children[0]->getAABBox().merge(children[1]->getAABBox());
		}
	}

}
