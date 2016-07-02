#ifndef ENGINE_AABBTREE_H
#define ENGINE_AABBTREE_H

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
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

		private:
			AABBNode *rootNode;

	};

}

#endif
