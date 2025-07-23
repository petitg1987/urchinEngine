#pragma once

#include <memory>
#include <vector>
#include <UrchinCommon.h>

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin {

    class CollisionConvexHullShape final : public CollisionShape3D {
        public:
            explicit CollisionConvexHullShape(const std::vector<Point3<float>>&);
            explicit CollisionConvexHullShape(std::unique_ptr<ConvexHullShape3D<float>>);
            CollisionConvexHullShape(CollisionConvexHullShape&&) noexcept;
            CollisionConvexHullShape(const CollisionConvexHullShape&) = delete;
            CollisionConvexHullShape operator=(const CollisionConvexHullShape&) = delete;
            ~CollisionConvexHullShape() override;

            ShapeType getShapeType() const override;
            const ConvexShape3D<float>& getSingleShape() const override;
            std::vector<Point3<float>> getPoints() const;

            std::unique_ptr<CollisionShape3D> scale(const Vector3<float>&) const override;

            AABBox<float> toAABBox(const PhysicsTransform&) const override;
            std::unique_ptr<CollisionConvexObject3D, ObjectDeleter> toConvexObject(const PhysicsTransform&) const override;

            Vector3<float> computeLocalInertia(float) const override;
            float getMaxDistanceToCenter() const override;
            float getMinDistanceToCenter() const override;

            std::unique_ptr<CollisionShape3D> clone() const override;

        private:
            void initialize();
            void initializeConvexHullReduced();
            void initializeDistances();

            std::unique_ptr<ConvexHullShape3D<float>> convexHullShape; //shape including margin
            std::unique_ptr<ConvexHullShape3D<float>> convexHullShapeReduced; //shape where margin has been subtracted

            float minDistanceToCenter;
            float maxDistanceToCenter;

            static thread_local std::unordered_map<const CollisionConvexHullShape*, AABBox<float>> aabboxCache;
            static thread_local std::unordered_map<const CollisionConvexHullShape*, PhysicsTransform> transformCache;
    };

}
