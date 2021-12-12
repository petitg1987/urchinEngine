#pragma once

#include <memory>

#include <UrchinAggregation.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class DefaultBodyShapeCreator {

        public:
            explicit DefaultBodyShapeCreator(const ObjectEntity&);

            std::unique_ptr<const CollisionShape3D> createDefaultBodyShape(CollisionShape3D::ShapeType) const;

        private:
            std::unique_ptr<ConvexHullShape3D<float>> buildConvexHullShape(const Model*) const;

            const ObjectEntity& objectEntity;
    };

}
