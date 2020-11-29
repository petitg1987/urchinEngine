#ifndef URCHINENGINE_SPHEREBOXCOLLISIONALGORITHM_H
#define URCHINENGINE_SPHEREBOXCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin {

    class SphereBoxCollisionAlgorithm : public CollisionAlgorithm {
        public:
            SphereBoxCollisionAlgorithm(bool, ManifoldResult &&);
            ~SphereBoxCollisionAlgorithm() override = default;

            void doProcessCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&) override;

            struct Builder : public CollisionAlgorithmBuilder {
                CollisionAlgorithm *createCollisionAlgorithm(bool, ManifoldResult &&, FixedSizePool<CollisionAlgorithm>*) const override;

                const std::vector<CollisionShape3D::ShapeType> &getFirstExpectedShapeType() const override;
                unsigned int getAlgorithmSize() const override;
            };
    };

}

#endif
