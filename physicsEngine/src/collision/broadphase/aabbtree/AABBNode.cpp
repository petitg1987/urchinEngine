#include "collision/broadphase/aabbtree/AABBNode.h"

namespace urchin
{

	//static
	float AABBNode::fatMargin = ConfigService::instance()->getFloatValue("broadPhase.aabbTreeFatMargin");

	AABBNode::AABBNode(BodyNodeData *bodyNodeData) :
		bodyNodeData(bodyNodeData),
		parentNode(nullptr),
		leftChild(nullptr),
		rightChild(nullptr)
	{

	}

	AABBNode::~AABBNode()
	{
		delete bodyNodeData;
	}

	bool AABBNode::isLeaf()
	{
		return leftChild==nullptr;
	}

	void AABBNode::setParent(AABBNode *parentNode)
	{
		this->parentNode = parentNode;
	}

	AABBNode *AABBNode::getParent() const
	{
		return parentNode;
	}

	void AABBNode::setChildren(AABBNode *leftChild, AABBNode *rightChild)
	{
		this->leftChild = leftChild;
		this->leftChild->setParent(this);

		this->rightChild = rightChild;
		this->rightChild->setParent(this);
	}

	AABBNode *AABBNode::getLeftChild() const
	{
		return leftChild;
	}

	AABBNode *AABBNode::getRightChild() const
	{
		return rightChild;
	}

	const AABBox<float> &AABBNode::getAABBox() const
	{
		return aabbox;
	}

	void AABBNode::updateAABBox()
	{
		if (isLeaf())
		{
			Point3<float> fatMargin3(fatMargin, fatMargin, fatMargin);
			AABBox<float> bodyBox = bodyNodeData->retrieveBodyAABBox();

			aabbox = AABBox<float>(bodyBox.getMin()-fatMargin3, bodyBox.getMax()+fatMargin3);
		}else
		{
			aabbox = leftChild->getAABBox().merge(rightChild->getAABBox());
		}
	}

}
