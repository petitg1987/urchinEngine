#pragma once

#include "shape/CollisionShape3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin {

    class TemporalObject {
        public:
            TemporalObject(const CollisionShape3D&, std::size_t, const PhysicsTransform&, const PhysicsTransform&);

            const CollisionShape3D& getShape() const;
            std::size_t getShapeIndex() const;
            const CollisionConvexObject3D& getLocalObject() const;

            const PhysicsTransform& getFrom() const;
            const PhysicsTransform& getTo() const;

            bool isRay() const;

        private:
            const CollisionShape3D& shape;
            std::size_t shapeIndex;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> localObject;

            PhysicsTransform from;
            PhysicsTransform to;
    };

}
