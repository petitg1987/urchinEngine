#ifndef URCHINENGINE_AABBNODE_H
#define URCHINENGINE_AABBNODE_H

#include "UrchinCommon.h"

#include "collision/broadphase/aabbtree/BodyNodeData.h"

namespace urchin
{

	class AABBNode
	{
		public:
			explicit AABBNode(BodyNodeData *);
			~AABBNode();

			BodyNodeData *getBodyNodeData() const;

			bool isLeaf() const;
			bool isRoot() const;

			void setParent(AABBNode *);
			AABBNode *getParent() const;

			void setLeftChild(AABBNode *);
			AABBNode *getLeftChild() const;
			void setRightChild(AABBNode *);
			AABBNode *getRightChild() const;

			AABBNode *getSibling() const;

			const AABBox<float> &getAABBox() const;
			void updateAABBox(float);

		private:
			BodyNodeData *bodyNodeData;
			AABBox<float> aabbox;

			AABBNode *parentNode;
			AABBNode *children[2];
	};

}

#endif
