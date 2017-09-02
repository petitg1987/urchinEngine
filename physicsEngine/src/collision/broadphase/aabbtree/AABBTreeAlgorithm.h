#ifndef URCHINENGINE_AABBTREEALGORITHM_H
#define URCHINENGINE_AABBTREEALGORITHM_H

#include "UrchinCommon.h"

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

			std::vector<AbstractWorkBody *> rayTest(const Ray<float> &) const;
			std::vector<AbstractWorkBody *> bodyTest(const AbstractWorkBody *, const PhysicsTransform &, const PhysicsTransform &) const;

		private:
			AABBTree *tree;

	};

}

#endif
