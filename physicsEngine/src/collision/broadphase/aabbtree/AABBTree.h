#ifndef ENGINE_AABBTREE_H
#define ENGINE_AABBTREE_H

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

			#ifdef _DEBUG
				void printTree(AABBNode *, unsigned int);
			#endif
	};

}

#endif
