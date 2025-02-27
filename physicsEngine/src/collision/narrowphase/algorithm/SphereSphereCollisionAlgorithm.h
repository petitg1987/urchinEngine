#pragma once

#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h>
#include <collision/ManifoldResult.h>
#include <collision/narrowphase/CollisionObjectWrapper.h>

namespace urchin {

    class SphereSphereCollisionAlgorithm final : public CollisionAlgorithm {
        public:
            SphereSphereCollisionAlgorithm(bool, const ManifoldResult&);
            ~SphereSphereCollisionAlgorithm() override = default;

            void doProcessCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&) override;

            struct Builder final : CollisionAlgorithmBuilder {
                CollisionAlgorithm* createCollisionAlgorithm(bool, const ManifoldResult&, FixedSizePool<CollisionAlgorithm>&) const override;

                const std::vector<CollisionShape3D::ShapeType>& getFirstExpectedShapeType() const override;
                unsigned int getAlgorithmSize() const override;
            };
    };

}
