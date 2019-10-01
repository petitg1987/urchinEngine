#ifndef URCHINENGINE_AABBNODE_H
#define URCHINENGINE_AABBNODE_H

#include "UrchinCommon.h"

#include "collision/broadphase/aabbtree/AABBNodeData.h"

namespace urchin
{

	template<class OBJ> class AABBNode
	{
		public:
			explicit AABBNode(AABBNodeData<OBJ> *);
			~AABBNode();

            AABBNodeData<OBJ> *getNodeData() const;

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
            AABBNodeData<OBJ> *nodeData;
			AABBox<float> aabbox;

			AABBNode<OBJ> *parentNode;
			AABBNode<OBJ> *children[2];
	};

    #include "AABBNode.inl"

}

#endif
