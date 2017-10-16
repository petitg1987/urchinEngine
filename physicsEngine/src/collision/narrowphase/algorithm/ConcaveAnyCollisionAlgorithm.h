#ifndef URCHINENGINE_CONCAVEANYCOLLISIONALGORITHM_H
#define URCHINENGINE_CONCAVEANYCOLLISIONALGORITHM_H

#include "collision/narrowphase/algorithm/CollisionAlgorithm.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmBuilder.h"
#include "collision/narrowphase/algorithm/CollisionAlgorithmSelector.h"
#include "collision/ManifoldResult.h"
#include "collision/narrowphase/CollisionObjectWrapper.h"

namespace urchin
{

    class ConcaveAnyCollisionAlgorithm : public CollisionAlgorithm
    {
        public:
            ConcaveAnyCollisionAlgorithm(bool, const ManifoldResult &);
            ~ConcaveAnyCollisionAlgorithm() override;

            void doProcessCollisionAlgorithm(const CollisionObjectWrapper &, const CollisionObjectWrapper &) override;

            struct Builder : public CollisionAlgorithmBuilder
            {
                CollisionAlgorithm *createCollisionAlgorithm(bool, const ManifoldResult &, void*) const override;

                std::set<CollisionShape3D::ShapeType> getFirstExpectedShapeType() const override;
                unsigned int getAlgorithmSize() const override;
            };

        private:
            CollisionAlgorithmSelector *const collisionAlgorithmSelector;
    };

}

#endif
