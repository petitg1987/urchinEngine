#pragma once

#include <memory>
#include <vector>
#include <set>
#include <mutex>
#include <UrchinCommon.h>

#include <utils/math/PhysicsTransform.h>
#include <object/CollisionConvexObject3D.h>
#include <utils/pool/FixedSizePool.h>
#include <object/pool/ObjectDeleter.h>
#include <object/pool/CollisionConvexObjectPool.h>

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

            static const std::vector<ShapeType>& convexShapes();
            static const std::vector<ShapeType>& concaveShapes();
            static const std::vector<ShapeType>& compoundShapes();
            static const std::vector<ShapeType>& sphereShapes();

            float getInnerMargin() const;
            virtual CollisionShape3D::ShapeType getShapeType() const = 0;
            bool isConvex() const;
            bool isConcave() const;
            bool isCompound() const;
            virtual const ConvexShape3D<float>* getSingleShape() const = 0;

            virtual std::shared_ptr<CollisionShape3D> scale(float) const = 0;

            virtual AABBox<float> toAABBox(const PhysicsTransform&) const = 0;
            virtual std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const = 0;

            virtual Vector3<float> computeLocalInertia(float) const = 0;
            virtual float getMaxDistanceToCenter() const = 0;
            virtual float getMinDistanceToCenter() const = 0;

            void checkInnerMarginQuality(const std::string&) const;

            virtual CollisionShape3D* clone() const = 0;

        protected:
            static FixedSizePool<CollisionConvexObject3D>* getObjectsPool();
            void refreshInnerMargin(float);

        private:
            float innerMargin;
            const float initialInnerMargin;
    };

}
