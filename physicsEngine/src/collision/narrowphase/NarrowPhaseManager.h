#ifndef URCHINENGINE_NARROWPHASEMANAGER_H
#define URCHINENGINE_NARROWPHASEMANAGER_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "collision/ManifoldResult.h"
#include "collision/OverlappingPair.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/continuous/result/ContinuousCollisionResult.h"
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

			void process(float, const std::vector<OverlappingPair *> &, std::vector<ManifoldResult> &);
			void processGhostBody(WorkGhostBody *, std::vector<ManifoldResult> &);

			ccd_set continuousCollisionTest(const TemporalObject &,  const std::vector<AbstractWorkBody *> &) const;
			ccd_set rayTest(const Ray<float> &, const std::vector<AbstractWorkBody *> &) const;

		private:
			void processOverlappingPairs(const std::vector<OverlappingPair *> &, std::vector<ManifoldResult> &);
			std::shared_ptr<CollisionAlgorithm> retrieveCollisionAlgorithm(OverlappingPair *);

			void processPredictiveContacts(float, std::vector<ManifoldResult> &);
			void handleContinuousCollision(AbstractWorkBody *, const PhysicsTransform &, const PhysicsTransform &, std::vector<ManifoldResult> &);
			void continuousCollisionTest(const TemporalObject &, const TemporalObject &, AbstractWorkBody *, ccd_set &) const;

			const BodyManager *bodyManager;
			const BroadPhaseManager *broadPhaseManager;

			CollisionAlgorithmSelector *const collisionAlgorithmSelector;

			const GJKContinuousCollisionAlgorithm<double, float> gjkContinuousCollisionAlgorithm;
	};

}

#endif
