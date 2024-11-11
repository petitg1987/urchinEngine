#pragma once

#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/ManifoldResult.h>
#include <shape/CollisionShape3D.h>
#include <utils/pool/FixedSizePool.h>

namespace urchin {

    class CollisionAlgorithmBuilder {
        public:
            virtual ~CollisionAlgorithmBuilder() = default;

            virtual CollisionAlgorithm* createCollisionAlgorithm(bool, const ManifoldResult&, FixedSizePool<CollisionAlgorithm>&) const = 0;

            virtual const std::vector<CollisionShape3D::ShapeType>& getFirstExpectedShapeType() const = 0;
            virtual unsigned int getAlgorithmSize() const = 0;
    };

}
