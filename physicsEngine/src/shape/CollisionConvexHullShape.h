#ifndef ENGINE_COLLISIONCONVEXHULLSHAPE_H
#define ENGINE_COLLISIONCONVEXHULLSHAPE_H

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
			CollisionConvexHullShape(float, const std::vector<Point3<float>> &);
			~CollisionConvexHullShape();

			CollisionShape3D::ShapeType getShapeType() const;
			const std::map<unsigned int, IndexedTriangle3D<float>> &getIndexedTriangles() const;
			const std::map<unsigned int, Point3<float>> &getIndexedPoints() const;
			std::vector<Point3<float>> getPoints() const;

			std::shared_ptr<CollisionShape3D> scale(float) const;

			AABBox<float> toAABBox(const PhysicsTransform &) const;
			std::shared_ptr<CollisionConvexObject3D> toConvexObject(const PhysicsTransform &) const;

			Vector3<float> computeLocalInertia(float) const;
			float getMaxDistanceToCenter() const;
			float getMinDistanceToCenter() const;

		private:
			void initialize();
			void initializeConvexHullReduced();
			void initializeDistances();

			ConvexHullShape3D<float> transformConvexHull(const ConvexHullShape3D<float> &, const PhysicsTransform &) const;

			const ConvexHullShape3D<float> convexHullShape; //shape including margin
			std::unique_ptr<ConvexHullShape3D<float>> convexHullShapeReduced; //shape where margin has been subtracted

			float minDistanceToCenter;
			float maxDistanceToCenter;
	};

}

#endif
