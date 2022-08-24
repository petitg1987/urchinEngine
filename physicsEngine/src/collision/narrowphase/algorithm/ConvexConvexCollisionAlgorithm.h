#pragma once

#include <UrchinCommon.h>

#include <collision/narrowphase/algorithm/CollisionAlgorithm.h>
#include <collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h>
#include <collision/ManifoldResult.h>
#include <collision/narrowphase/CollisionObjectWrapper.h>
#include <collision/narrowphase/algorithm/epa/EPAAlgorithm.h>

namespace urchin {

    class ConvexConvexCollisionAlgorithm final : public CollisionAlgorithm {
        public:
            ConvexConvexCollisionAlgorithm(bool, const ManifoldResult&);
            ~ConvexConvexCollisionAlgorithm() override = default;

            void doProcessCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&) override;

            struct Builder : public CollisionAlgorithmBuilder {
                CollisionAlgorithm* createCollisionAlgorithm(bool, const ManifoldResult&, FixedSizePool<CollisionAlgorithm>&) const override;

                const std::vector<CollisionShape3D::ShapeType>& getFirstExpectedShapeType() const override;
                unsigned int getAlgorithmSize() const override;
            };

        private:
            void processCollisionAlgorithmWithMargin(const CollisionConvexObject3D&, const CollisionConvexObject3D&);

            GJKAlgorithm<double> gjkAlgorithm;
            EPAAlgorithm<double> epaAlgorithm;
    };

}
