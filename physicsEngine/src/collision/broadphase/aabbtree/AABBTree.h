#ifndef URCHINENGINE_AABBTREE_H
#define URCHINENGINE_AABBTREE_H

#include "UrchinCommon.h"

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/AABBNode.h"

namespace urchin
{

	class AABBTree
	{
		public:
			AABBTree();
			~AABBTree();

			void addBody(AbstractWorkBody *, PairContainer *);
			void removeBody(AbstractWorkBody *);
			void updateBodies();

			const std::vector<OverlappingPair *> &getOverlappingPairs() const;

			void rayTest(const Ray<float> &, std::vector<AbstractWorkBody *> &) const;
			void enlargedRayTest(const Ray<float> &, float, const AbstractWorkBody *, std::vector<AbstractWorkBody *> &) const;

		private:
			void insertNode(AABBNode *, AABBNode *);
			void replaceNode(AABBNode *, AABBNode *);
			void removeNode(AABBNode *);

			void computeOverlappingPairsFor(AABBNode *, AABBNode *);
			void createOverlappingPair(BodyNodeData *, BodyNodeData *);
			void removeOverlappingPairs(const BodyNodeData *);

			const float fatMargin;

			AABBNode *rootNode;
			std::map<AbstractWorkBody *, AABBNode *> bodiesNode;
			PairContainer *defaultPairContainer;

			mutable std::vector<AABBNode *> browseNodes;

			#ifdef _DEBUG
				void printTree(AABBNode *, unsigned int);
			#endif
	};

}

#endif
