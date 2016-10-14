#ifndef ENGINE_BROADPHASEMANAGER_H
#define ENGINE_BROADPHASEMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "collision/broadphase/BroadPhaseAlgorithm.h"
#include "collision/broadphase/PairContainer.h"
#include "collision/OverlappingPair.h"
#include "body/BodyManager.h"

namespace urchin
{

	/**
	* Broad phase manager allowing to determine pairs of bodies potentially colliding
	*/
	class BroadPhaseManager : public Observer
	{
		public:
			BroadPhaseManager(BodyManager *);
			~BroadPhaseManager();

			void notify(Observable *, int);

			void addBody(AbstractWorkBody *, PairContainer *);
			void removeBody(AbstractWorkBody *);

			const std::vector<OverlappingPair *> &computeOverlappingPairs();

			std::vector<AbstractWorkBody *> rayTest(const Ray<float> &) const;
			std::vector<AbstractWorkBody *> enlargedRayTest(const Ray<float> &, const Vector3<float> &) const;

		private:
			BroadPhaseAlgorithm *broadPhaseAlgorithm;
	};

}

#endif
