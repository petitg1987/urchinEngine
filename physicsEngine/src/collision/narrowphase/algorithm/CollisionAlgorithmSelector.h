#pragma once

#include <memory>

#include <body/model/AbstractBody.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h>
#include <utils/pool/SyncFixedSizePool.h>

namespace urchin {

    class AlgorithmDeleter {
        public:
            AlgorithmDeleter() = default;
            explicit AlgorithmDeleter(std::shared_ptr<FixedSizePool<CollisionAlgorithm>>);
            void operator()(CollisionAlgorithm*) const;

        private:
            std::shared_ptr<FixedSizePool<CollisionAlgorithm>> algorithmPool;
    };

    class CollisionAlgorithmSelector {
        public:
            CollisionAlgorithmSelector();

            std::unique_ptr<CollisionAlgorithm, AlgorithmDeleter> createCollisionAlgorithm(
                    AbstractBody&, const CollisionShape3D&, AbstractBody&, const CollisionShape3D&) const;

        private:
            void initializeCollisionAlgorithmBuilderMatrix();
            void initializeConcaveAlgorithm();
            void initializeCompoundAlgorithm();

            void initializeAlgorithmPool();

            std::shared_ptr<SyncFixedSizePool<CollisionAlgorithm>> algorithmPool;
            std::array<std::array<std::unique_ptr<CollisionAlgorithmBuilder>, CollisionShape3D::SHAPE_MAX>, CollisionShape3D::SHAPE_MAX> collisionAlgorithmBuilderMatrix;
    };

}
