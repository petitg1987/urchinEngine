#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <UrchinCommon.h>

#include <collision/ManifoldResult.h>
#include <collision/OverlappingPair.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithmSelector.h>
#include <collision/narrowphase/algorithm/continuous/GJKContinuousCollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/continuous/result/ContinuousCollisionResult.h>
#include <collision/broadphase/BroadPhaseManager.h>
#include <body/BodyManager.h>
#include <body/model/AbstractBody.h>
#include <body/model/GhostBody.h>
#include <object/TemporalObject.h>
#include <shape/CollisionTriangleShape.h>

namespace urchin {
    class NarrowPhaseManager {
        public:
            NarrowPhaseManager(const BodyManager*, const BroadPhaseManager*);
            ~NarrowPhaseManager();

            void process(float, const std::vector<OverlappingPair*>&, std::vector<ManifoldResult>&);
            void processGhostBody(GhostBody*, std::vector<ManifoldResult>&);

            ccd_set continuousCollisionTest(const TemporalObject&,  const std::vector<AbstractBody*>&) const;
            ccd_set rayTest(const Ray<float>&, const std::vector<AbstractBody*>&) const;

        private:
            void processOverlappingPairs(const std::vector<OverlappingPair*>&, std::vector<ManifoldResult>&);
            void processOverlappingPair(OverlappingPair*, std::vector<ManifoldResult>&);
            std::shared_ptr<CollisionAlgorithm> retrieveCollisionAlgorithm(OverlappingPair*);

            void processPredictiveContacts(float, std::vector<ManifoldResult>&);
            void handleContinuousCollision(AbstractBody*, const PhysicsTransform&, const PhysicsTransform&, std::vector<ManifoldResult>&);
            void trianglesContinuousCollisionTest(const std::vector<CollisionTriangleShape>&, const TemporalObject&, AbstractBody*, ccd_set&) const;
            void continuousCollisionTest(const TemporalObject&, const TemporalObject&, AbstractBody*, ccd_set&) const;

            const BodyManager* bodyManager;
            const BroadPhaseManager* broadPhaseManager;

            CollisionAlgorithmSelector *const collisionAlgorithmSelector;
            const GJKContinuousCollisionAlgorithm<double, float> gjkContinuousCollisionAlgorithm;

            std::shared_ptr<LockById> bodiesMutex;
    };

}
