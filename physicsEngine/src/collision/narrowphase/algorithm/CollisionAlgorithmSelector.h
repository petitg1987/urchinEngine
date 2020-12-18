#ifndef URCHINENGINE_COLLISIONALGORITHMSELECTOR_H
#define URCHINENGINE_COLLISIONALGORITHMSELECTOR_H

#include <memory>

#include "body/model/AbstractBody.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "utils/pool/SyncFixedSizePool.h"

namespace urchin {

    class CollisionAlgorithmSelector {
        public:
            CollisionAlgorithmSelector();
            ~CollisionAlgorithmSelector();

            std::shared_ptr<CollisionAlgorithm> createCollisionAlgorithm(AbstractBody*, const CollisionShape3D*,
                                                                         AbstractBody*, const CollisionShape3D*) const;

        private:
            void initializeCollisionAlgorithmBuilderMatrix();
            void initializeConcaveAlgorithm();
            void initializeCompoundAlgorithm();
            void deleteCollisionAlgorithmBuilderMatrix();

            void initializeAlgorithmPool();

            class AlgorithmDeleter {
                public:
                    explicit AlgorithmDeleter(FixedSizePool<CollisionAlgorithm>*);
                    void operator()(CollisionAlgorithm*);

                private:
                    FixedSizePool<CollisionAlgorithm> *const algorithmPool;
            };

            SyncFixedSizePool<CollisionAlgorithm>* algorithmPool;
            CollisionAlgorithmBuilder *collisionAlgorithmBuilderMatrix[CollisionShape3D::SHAPE_MAX][CollisionShape3D::SHAPE_MAX];
    };

}

#endif
