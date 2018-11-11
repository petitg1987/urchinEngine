#include <limits>
#include <cmath>
#include <cassert>

#include "shape/CollisionConvexHullShape.h"
#include "object/CollisionConvexHullObject.h"

namespace urchin
{

	/**
	* @param points Points used to construct the convex hull
	*/
	CollisionConvexHullShape::CollisionConvexHullShape(const std::vector<Point3<float>> &points) :
			CollisionShape3D(),
			convexHullShape(std::make_shared<ConvexHullShape3D<float>>(points))
	{
		initialize();
	}

	CollisionConvexHullShape::CollisionConvexHullShape(const std::shared_ptr<ConvexHullShape3D<float>> &convexHullShape) :
			CollisionShape3D(),
			convexHullShape(convexHullShape)
	{
		initialize();
	}

	void CollisionConvexHullShape::initialize()
	{
		lastTransform.setPosition(Point3<float>(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()));

		initializeDistances();
		initializeConvexHullReduced();
	}

	void CollisionConvexHullShape::initializeConvexHullReduced()
	{
		if(minDistanceToCenter > getInnerMargin())
		{
			convexHullShapeReduced = convexHullShape->resize(-getInnerMargin());
		}else
		{ //impossible to shrink the convex hull correctly
			refreshInnerMargin(0.0f);
		}
	}

	void CollisionConvexHullShape::initializeDistances()
	{
		AABBox<float> aabbox = toAABBox(PhysicsTransform());
		maxDistanceToCenter = aabbox.getMaxHalfSize();
		minDistanceToCenter = aabbox.getMinHalfSize();
	}

	CollisionShape3D::ShapeType CollisionConvexHullShape::getShapeType() const
	{
		return CollisionShape3D::CONVEX_HULL_SHAPE;
	}

	std::shared_ptr<ConvexShape3D<float>> CollisionConvexHullShape::getSingleShape() const
	{
		return convexHullShape;
	}

	/**
	 * @return Points including margin of the convex hull
	 */
	std::vector<Point3<float>> CollisionConvexHullShape::getPoints() const
	{
		return convexHullShape->getPoints();
	}

	std::shared_ptr<CollisionShape3D> CollisionConvexHullShape::scale(float scale) const
	{
		const std::vector<Point3<float>> &convexHullPoints = convexHullShape->getPoints();

		std::vector<Point3<float>> newPoints;
		newPoints.reserve(convexHullPoints.size());

		for (const auto &convexHullPoint : convexHullPoints)
		{
			newPoints.push_back(convexHullPoint * scale);
		}

		return std::make_shared<CollisionConvexHullShape>(newPoints);
	}

	AABBox<float> CollisionConvexHullShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		if(!lastTransform.equals(physicsTransform))
		{
			const Quaternion<float> &orientation = physicsTransform.getOrientation();
			Point3<float> min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
			Point3<float> max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
			const std::vector<Point3<float>> &convexHullShapePoints = convexHullShape->getPoints();
			for (const auto &convexHullShapePoint : convexHullShapePoints)
			{
				const Point3<float> point = orientation.rotatePoint(convexHullShapePoint);

				if (min.X > point.X)
				{
					min.X = point.X;
				}
				if (min.Y > point.Y)
				{
					min.Y = point.Y;
				}
				if (min.Z > point.Z)
				{
					min.Z = point.Z;
				}

				if (max.X < point.X)
				{
					max.X = point.X;
				}
				if (max.Y < point.Y)
				{
					max.Y = point.Y;
				}
				if (max.Z < point.Z)
				{
					max.Z = point.Z;
				}
			}

			const Point3<float> &position = physicsTransform.getPosition();

			lastAABBox = AABBox<float>(min + position, max + position);
			lastTransform = physicsTransform;
		}
		return lastAABBox;
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionConvexHullShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		Transform<float> transform = physicsTransform.toTransform();
        auto convexHullWithMargin = std::shared_ptr<ConvexHull3D<float>>(dynamic_cast<ConvexHull3D<float>*>(convexHullShape->toConvexObject(transform).release()));

		if(!convexHullShapeReduced)
		{ //impossible to compute convex hull without margin => use convex hull with margin and set a margin of 0.0
			#ifdef _DEBUG
            	assert(getInnerMargin()==0.0f);
    		#endif
            std::shared_ptr<ConvexHull3D<float>> convexHullWithoutMargin(convexHullWithMargin);
            return std::make_shared<CollisionConvexHullObject>(getInnerMargin(), convexHullWithMargin, convexHullWithoutMargin);
		}

        auto convexHullWithoutMargin = std::shared_ptr<ConvexHull3D<float>>(dynamic_cast<ConvexHull3D<float>*>(convexHullShapeReduced->toConvexObject(transform).release()));
		return std::make_shared<CollisionConvexHullObject>(getInnerMargin(), convexHullWithMargin, convexHullWithoutMargin);
	}

	Vector3<float> CollisionConvexHullShape::computeLocalInertia(float mass) const
	{
		AABBox<float> aabbox = toAABBox(PhysicsTransform());
		float width = 2.0f * aabbox.getHalfSize(0);
		float height = 2.0f * aabbox.getHalfSize(1);
		float depth = 2.0f * aabbox.getHalfSize(2);

		float localInertia1 = (1.0f/12.0f) * mass * (height*height + depth*depth);
		float localInertia2 = (1.0f/12.0f) * mass * (width*width + depth*depth);
		float localInertia3 = (1.0f/12.0f) * mass * (width*width + height*height);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

	float CollisionConvexHullShape::getMaxDistanceToCenter() const
	{
		return maxDistanceToCenter;
	}

	float CollisionConvexHullShape::getMinDistanceToCenter() const
	{
		return minDistanceToCenter;
	}

	CollisionShape3D *CollisionConvexHullShape::clone() const
	{
		return new CollisionConvexHullShape(convexHullShape->getPoints());
	}

}
