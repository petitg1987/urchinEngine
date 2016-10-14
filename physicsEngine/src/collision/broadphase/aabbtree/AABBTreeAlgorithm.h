#ifndef ENGINE_AABBTREEALGORITHM_H
#define ENGINE_AABBTREEALGORITHM_H

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
			std::vector<AbstractWorkBody *> enlargedRayTest(const Ray<float> &, const Vector3<float> &) const;

		private:
			AABBTree *tree;

	};

}

#endif
