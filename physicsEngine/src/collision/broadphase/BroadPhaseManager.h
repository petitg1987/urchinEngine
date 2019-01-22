#ifndef URCHINENGINE_BROADPHASEMANAGER_H
#define URCHINENGINE_BROADPHASEMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/OverlappingPair.h"
#include "body/work/AbstractWorkBody.h"
#include "body/BodyManager.h"

namespace urchin
{

	/**
	* Broad phase manager allowing to determine pairs of bodies potentially colliding
	*/
	class BroadPhaseManager : public Observer
	{
		public:
			explicit BroadPhaseManager(BodyManager *);
			~BroadPhaseManager() override;

			void notify(Observable *, int) override;

			void addBody(AbstractWorkBody *);
			void removeBody(AbstractWorkBody *);

			const std::vector<OverlappingPair *> &computeOverlappingPairs();

			std::vector<AbstractWorkBody *> rayTest(const Ray<float> &) const;
			std::vector<AbstractWorkBody *> bodyTest(const AbstractWorkBody *, const PhysicsTransform &, const PhysicsTransform &) const;

		private:
			BroadPhaseAlgorithm *broadPhaseAlgorithm;
	};

}

#endif
