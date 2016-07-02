#ifndef ENGINE_AABBTREEALGORITHM_H
#define ENGINE_AABBTREEALGORITHM_H

#include "body/work/AbstractWorkBody.h"
#include "collision/OverlappingPair.h"
#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/aabbtree/AABBTree.h"

namespace urchin
{

	class AABBTreeAlgorithm : public BroadPhaseAlgorithm
	{
		public:
			AABBTreeAlgorithm();
			~AABBTreeAlgorithm();

			void addBody(AbstractWorkBody *, PairContainer *);
			void removeBody(AbstractWorkBody *);
			void updateBodies();

			const std::vector<OverlappingPair *> &getOverlappingPairs() const;

		private:
			AABBTree *tree;

	};

}

#endif
