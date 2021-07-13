#pragma once

#include <memory>

#include <UrchinMapHandler.h>
#include <UrchinPhysicsEngine.h>

namespace urchin {

    class DefaultBodyShapeCreator {

        public:
            explicit DefaultBodyShapeCreator(const SceneObject&);

            std::unique_ptr<const CollisionShape3D> createDefaultBodyShape(CollisionShape3D::ShapeType) const;

        private:
            std::unique_ptr<ConvexHullShape3D<float>> buildConvexHullShape(const Model*) const;

            const SceneObject& sceneObject;
    };

}
