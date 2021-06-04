#pragma once

#include <shape/CollisionShape3D.h>
#include <utils/math/PhysicsTransform.h>

namespace urchin {

    class TemporalObject {
        public:
            TemporalObject(const CollisionShape3D*, const PhysicsTransform&, const PhysicsTransform&);

            const CollisionShape3D* getShape() const;
            const std::unique_ptr<CollisionConvexObject3D, ObjectDeleter>& getLocalObject() const;

            const PhysicsTransform& getFrom() const;
            const PhysicsTransform& getTo() const;

            bool isRay() const;

        private:
            const CollisionShape3D* shape;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> localObject;

            PhysicsTransform from;
            PhysicsTransform to;
    };

}
