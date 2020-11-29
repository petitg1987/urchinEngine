#ifndef URCHINENGINE_CONEXCONVEXCOLLISIONALGORITHM_H
#define URCHINENGINE_CONEXCONVEXCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"
#include "collision/narrowphase/algorithm/gjk/GJKAlgorithm.h"
#include "collision/narrowphase/algorithm/epa/EPAAlgorithm.h"

namespace urchin {

    class ConvexConvexCollisionAlgorithm : public CollisionAlgorithm {
        public:
            ConvexConvexCollisionAlgorithm(bool, ManifoldResult&&);
            ~ConvexConvexCollisionAlgorithm() override = default;

            void doProcessCollisionAlgorithm(const CollisionObjectWrapper&, const CollisionObjectWrapper&) override;

            struct Builder : public CollisionAlgorithmBuilder {
                CollisionAlgorithm* createCollisionAlgorithm(bool, ManifoldResult&&, FixedSizePool<CollisionAlgorithm>*) const override;

                const std::vector<CollisionShape3D::ShapeType>& getFirstExpectedShapeType() const override;
                unsigned int getAlgorithmSize() const override;
            };

        private:
            void processCollisionAlgorithmWithMargin(const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&,
                    const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>&);

            GJKAlgorithm<double> gjkAlgorithm;
            EPAAlgorithm<double> epaAlgorithm;
    };

}

#endif
