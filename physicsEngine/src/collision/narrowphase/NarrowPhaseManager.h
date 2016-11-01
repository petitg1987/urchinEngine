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
#include "collision/broadphase/BroadPhaseManager.h"
#include "body/BodyManager.h"
#include "body/work/AbstractWorkBody.h"
#include "body/work/WorkGhostBody.h"
#include "object/TemporalObject.h"

namespace urchin
{
	class NarrowPhaseManager
	{
		public:
			NarrowPhaseManager(const BodyManager *, const BroadPhaseManager *);
			~NarrowPhaseManager();

			std::vector<ManifoldResult> *process(float, const std::vector<OverlappingPair *> &);
			std::vector<ManifoldResult> *processGhostBody(WorkGhostBody *);

			ccd_set continuousCollissionTest(const TemporalObject &,  const std::vector<AbstractWorkBody *> &) const;
			ccd_set rayTest(const Ray<float> &, const std::vector<AbstractWorkBody *> &) const;

		private:
			void processOverlappingPairs(const std::vector<OverlappingPair *> &);
			std::shared_ptr<CollisionAlgorithm> retrieveCollisionAlgorithm(OverlappingPair *overlappingPair);

			void processPredictiveContacts(float);
			void handleContinuousCollision(AbstractWorkBody *, const PhysicsTransform &, const PhysicsTransform &, float);

			const BodyManager *bodyManager;
			const BroadPhaseManager *broadPhaseManager;

			CollisionAlgorithmSelector *collisionAlgorithmSelector;
			std::vector<ManifoldResult> *manifoldResults;

			GJKContinuousCollisionAlgorithm<double, float> gjkContinuousCollisionAlgorithm;
	};

}

#endif
