#ifndef ENGINE_NARROWPHASEMANAGER_H
#define ENGINE_NARROWPHASEMANAGER_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "collision/ManifoldResult.h"
#include "collision/OverlappingPair.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/continuous/ContinuousCollisionResult.h"
#include "body/work/AbstractWorkBody.h"

namespace urchin
{

	class NarrowPhaseManager
	{
		public:
			NarrowPhaseManager();
			~NarrowPhaseManager();

			std::vector<ManifoldResult> *process(const std::vector<OverlappingPair *> &);

			std::vector<std::shared_ptr<ContinuousCollisionResult<double>>> rayTest(const Ray<float> &, const std::vector<AbstractWorkBody *> &) const;

		private:
			std::shared_ptr<CollisionAlgorithm> retrieveCollisionAlgorithm(OverlappingPair *overlappingPair);

			CollisionAlgorithmSelector *collisionAlgorithmSelector;
			std::vector<ManifoldResult> *manifoldResults;

			GJKContinuousCollisionAlgorithm<double> gjkContinuousCollisionAlgorithm;
	};

}

#endif
