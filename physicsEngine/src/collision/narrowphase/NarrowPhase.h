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
#include <collision/broadphase/BroadPhase.h>
#include <body/BodyContainer.h>
#include <body/model/AbstractBody.h>
#include <body/model/GhostBody.h>
#include <object/TemporalObject.h>
#include <shape/CollisionTriangleShape.h>

namespace urchin {
    class NarrowPhase {
        public:
            NarrowPhase(const BodyContainer&, const BroadPhase&);

            void process(float, const std::vector<std::unique_ptr<OverlappingPair>>&, std::vector<ManifoldResult>&);
            void processGhostBody(const GhostBody&, std::vector<ManifoldResult>&);

            ccd_set continuousCollisionTest(const TemporalObject&, const std::vector<std::shared_ptr<AbstractBody>>&) const;
            ccd_set rayTest(const Ray<float>&, const std::vector<std::shared_ptr<AbstractBody>>&) const;

        private:
            void processOverlappingPairs(const std::vector<std::unique_ptr<OverlappingPair>>&, std::vector<ManifoldResult>&);
            void processOverlappingPair(OverlappingPair&, std::vector<ManifoldResult>&);
            CollisionAlgorithm* retrieveCollisionAlgorithm(OverlappingPair&);

            void processPredictiveContacts(float, std::vector<ManifoldResult>&) const;
            void handleContinuousCollision(AbstractBody&, const PhysicsTransform&, const PhysicsTransform&, std::vector<ManifoldResult>&) const;
            void trianglesContinuousCollisionTest(const std::vector<CollisionTriangleShape>&, const TemporalObject&, const std::shared_ptr<AbstractBody>&, ccd_set&) const;
            void continuousCollisionTest(const TemporalObject&, const TemporalObject&, std::shared_ptr<AbstractBody>, ccd_set&) const;

            const BodyContainer& bodyContainer;
            const BroadPhase& broadPhase;

            CollisionAlgorithmSelector collisionAlgorithmSelector;
            GJKContinuousCollisionAlgorithm<double, float> gjkContinuousCollisionAlgorithm;

            std::shared_ptr<LockById> bodiesMutex;

            static thread_local std::vector<OverlappingPair> overlappingPairsCache;
    };

}
