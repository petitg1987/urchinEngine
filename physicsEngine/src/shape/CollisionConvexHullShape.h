#ifndef URCHINENGINE_COLLISIONCONVEXHULLSHAPE_H
#define URCHINENGINE_COLLISIONCONVEXHULLSHAPE_H

#include <memory>
#include <vector>
#include "UrchinCommon.h"

#include "shape/CollisionShape3D.h"
#include "object/CollisionConvexObject3D.h"
#include "utils/math/PhysicsTransform.h"

namespace urchin
{

	class CollisionConvexHullShape : public CollisionShape3D
	{
		public:
			explicit CollisionConvexHullShape(const std::vector<Point3<float>> &);
			explicit CollisionConvexHullShape(const std::shared_ptr<ConvexHullShape3D<float>> &);

			CollisionShape3D::ShapeType getShapeType() const override;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const override;
			std::vector<Point3<float>> getPoints() const;

			std::shared_ptr<CollisionShape3D> scale(float) const override;

			AABBox<float> toAABBox(const PhysicsTransform &) const override;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const override;

			Vector3<float> computeLocalInertia(float) const override;
			float getMaxDistanceToCenter() const override;
			float getMinDistanceToCenter() const override;

		private:
			void initialize();
			void initializeConvexHullReduced();
			void initializeDistances();

			const std::shared_ptr<ConvexHullShape3D<float>> convexHullShape; //shape including margin
			std::unique_ptr<ConvexHullShape3D<float>> convexHullShapeReduced; //shape where margin has been subtracted

			float minDistanceToCenter;
			float maxDistanceToCenter;

			mutable AABBox<float> lastAABBox;
			mutable PhysicsTransform lastTransform;
	};

}

#endif
