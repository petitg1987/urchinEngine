#ifndef ENGINE_AABBNODE_H
#define ENGINE_AABBNODE_H

#include "UrchinCommon.h"

#include "collision/broadphase/aabbtree/BodyNodeData.h"

namespace urchin
{

	class AABBNode
	{
		public:
			AABBNode(BodyNodeData *);
			~AABBNode();

			bool isLeaf();

			void setParent(AABBNode *);
			AABBNode *getParent() const;

			void setChildren(AABBNode *, AABBNode *);
			AABBNode *getLeftChild() const;
			AABBNode *getRightChild() const;

			const AABBox<float> &getAABBox() const;
			void updateAABBox();

		private:
			static float fatMargin;

			BodyNodeData *bodyNodeData;
			AABBox<float> aabbox;

			AABBNode *parentNode;
			AABBNode *leftChild;
			AABBNode *rightChild;
	};

}

#endif
