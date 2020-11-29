#ifndef URCHINENGINE_COLLISIONSHAPE3D_H
#define URCHINENGINE_COLLISIONSHAPE3D_H

#include <memory>
#include <vector>
#include <set>
#include <mutex>
#include "UrchinCommon.h"

#include "utils/math/PhysicsTransform.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/pool/FixedSizePool.h"
#include "object/pool/ObjectDeleter.h"

namespace urchin {

    class CollisionShape3D {
        public:
            CollisionShape3D();
            explicit CollisionShape3D(float);
            CollisionShape3D(const CollisionShape3D&) = default;
            virtual ~CollisionShape3D() = default;

            enum ShapeType {
                //Convex:
                SPHERE_SHAPE = 0,
                BOX_SHAPE,
                CAPSULE_SHAPE,
                CYLINDER_SHAPE,
                CONE_SHAPE,
                CONVEX_HULL_SHAPE,
                TRIANGLE_SHAPE,
                //compound:
                COMPOUND_SHAPE,
                //Concave:
                HEIGHTFIELD_SHAPE,

                SHAPE_MAX
            };
            static std::vector<ShapeType> CONVEX_SHAPES, CONCAVE_SHAPES, COMPOUND_SHAPES, SPHERE_SHAPES;

            float getInnerMargin() const;
            virtual CollisionShape3D::ShapeType getShapeType() const = 0;
            bool isConvex() const;
            bool isConcave() const;
            bool isCompound() const;
            virtual const ConvexShape3D<float> *getSingleShape() const = 0;

            virtual std::shared_ptr<CollisionShape3D> scale(float) const = 0;

            virtual AABBox<float> toAABBox(const PhysicsTransform&) const = 0;
            virtual std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const = 0;

            virtual Vector3<float> computeLocalInertia(float) const = 0;
            virtual float getMaxDistanceToCenter() const = 0;
            virtual float getMinDistanceToCenter() const = 0;

            void checkInnerMarginQuality(const std::string&) const;

            virtual CollisionShape3D *clone() const = 0;

        protected:
            FixedSizePool<CollisionConvexObject3D> *getObjectsPool() const;
            void refreshInnerMargin(float);

            mutable AABBox<float> lastAABBox;
            mutable PhysicsTransform lastTransform;

        private:
            float innerMargin;
            const float initialInnerMargin;
    };

}

#endif
