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
			CollisionConvexHullShape(const std::vector<Point3<float>> &);

			CollisionShape3D::ShapeType getShapeType() const;
			std::shared_ptr<ConvexShape3D<float>> getSingleShape() const;
			std::vector<Point3<float>> getPoints() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void initializeConvexHullReduced();
			void initializeDistances();

			const std::shared_ptr<ConvexHullShape3D<float>> convexHullShape; //shape including margin
			std::unique_ptr<ConvexHullShape3D<float>> convexHullShapeReduced; //shape where margin has been subtracted

			float minDistanceToCenter;
			float maxDistanceToCenter;
	};

}

#endif
