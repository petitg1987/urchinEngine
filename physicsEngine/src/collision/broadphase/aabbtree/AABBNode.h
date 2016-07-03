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

			BodyNodeData *getBodyNodeData() const;

			bool isLeaf();

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
			AABBox<float> aabbox;

			union
			{
				AABBNode *children[2];
				BodyNodeData *bodyNodeData;
			};
			AABBNode *parentNode;
	};

}

#endif
