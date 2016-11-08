#include <limits>

#include "shape/CollisionConvexHullShape.h"
#include "shape/util/ResizeConvexHullService.h"
#include "shape/CollisionSphereShape.h"
#include "object/CollisionConvexHullObject.h"

namespace urchin
{

	/**
	* @param points Points used to construct the convex hull
	*/
	CollisionConvexHullShape::CollisionConvexHullShape(const std::vector<Point3<float>> &points) :
			CollisionShape3D(),
			convexHull(ConvexHull3D<float>(points))
	{
		initialize();
	}

	/**
	* @param points Points used to construct the convex hull
	*/
	CollisionConvexHullShape::CollisionConvexHullShape(float innerMargin, const std::vector<Point3<float>> &points) :
			CollisionShape3D(innerMargin),
			convexHull(ConvexHull3D<float>(points))
	{
		initialize();
	}

	CollisionConvexHullShape::~CollisionConvexHullShape()
	{

	}

	void CollisionConvexHullShape::initialize()
	{
		initializeConvexHullReduced();
		initializeSphereShape();
	}

	void CollisionConvexHullShape::initializeConvexHullReduced()
	{
		convexHullReduced = ResizeConvexHullService::instance()->resizeConvexHull(convexHull, -getInnerMargin());

		if(convexHullReduced.get()==nullptr)
		{ //impossible to shrink the convex hull correctly
			refreshInnerMargin(0.0);
		}
	}

	void CollisionConvexHullShape::initializeSphereShape()
	{
		AABBox<float> aabbox = toAABBox(PhysicsTransform());
		Point3<float> boxCenterPoint = aabbox.getCenterPoint();

		const std::vector<Point3<float>> &convexHullPoints = convexHull.getPoints();
		float minSquareDistance = std::numeric_limits<float>::max();
		for(std::vector<Point3<float>>::const_iterator it=convexHullPoints.begin(); it!=convexHullPoints.end(); ++it)
		{
			float squareDistance = boxCenterPoint.squareDistance(*it);
			if(squareDistance < minSquareDistance)
			{
				minSquareDistance = squareDistance;
			}
		}

		confinedSphereShape = std::make_shared<CollisionSphereShape>(std::sqrt(minSquareDistance));
	}

	CollisionShape3D::ShapeType CollisionConvexHullShape::getShapeType() const
	{
		return CollisionShape3D::CONVEX_HULL_SHAPE;
	}

	const std::map<unsigned int, IndexedTriangle3D<float>> &CollisionConvexHullShape::getIndexedTriangles() const
	{
		return convexHull.getIndexedTriangles();
	}

	const std::map<unsigned int, Point3<float>> &CollisionConvexHullShape::getIndexedPoints() const
	{
		return convexHull.getIndexedPoints();
	}

	/**
	 * @return Points including margin of the convex hull
	 */
	std::vector<Point3<float>> CollisionConvexHullShape::getPoints() const
	{
		return convexHull.getPoints();
	}

	std::shared_ptr<CollisionShape3D> CollisionConvexHullShape::scale(float scale) const
	{
		const std::vector<Point3<float>> &convexHullPoints = convexHull.getPoints();

		std::vector<Point3<float>> newPoints;
		newPoints.reserve(convexHullPoints.size());

		for(std::vector<Point3<float>>::const_iterator it=convexHullPoints.begin(); it!=convexHullPoints.end(); ++it)
		{
			newPoints.push_back((*it) * scale);
		}

		return std::make_shared<CollisionConvexHullShape>(newPoints);
	}

	AABBox<float> CollisionConvexHullShape::toAABBox(const PhysicsTransform &physicsTransform) const
	{
		const Point3<float> &pos = physicsTransform.getPosition();
		const Quaternion<float> &orientation = physicsTransform.getOrientation();

		//build AABBox
		Point3<float> min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Point3<float> max(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
		const std::vector<Point3<float>> &convexHullPoints = convexHull.getPoints();
		for(std::vector<Point3<float>>::const_iterator it=convexHullPoints.begin(); it!=convexHullPoints.end(); ++it)
		{
			const Point3<float> point = orientation.rotatePoint(*it);

			if(min.X > point.X)
			{
				min.X = point.X;
			}
			if(min.Y > point.Y)
			{
				min.Y = point.Y;
			}
			if(min.Z > point.Z)
			{
				min.Z = point.Z;
			}

			if(max.X < point.X)
			{
				max.X = point.X;
			}
			if(max.Y < point.Y)
			{
				max.Y = point.Y;
			}
			if(max.Z < point.Z)
			{
				max.Z = point.Z;
			}
		}

		return AABBox<float>(min + pos, max + pos);
	}

	std::shared_ptr<CollisionSphereShape> CollisionConvexHullShape::toConfinedSphereShape() const
	{
		return confinedSphereShape;
	}

	std::shared_ptr<CollisionConvexObject3D> CollisionConvexHullShape::toConvexObject(const PhysicsTransform &physicsTransform) const
	{
		ConvexHull3D<float> convexHullWithMargin = transformConvexHull(convexHull, physicsTransform);

		if(convexHullReduced.get()==nullptr)
		{ //impossible to compute convex hull without margin => use convex hull with margin and set a margin of 0.0
			return std::make_shared<CollisionConvexHullObject>(getInnerMargin(), convexHullWithMargin, convexHullWithMargin);
		}

		ConvexHull3D<float> convexHullWithoutMargin = transformConvexHull(*convexHullReduced.get(), physicsTransform);
		return std::make_shared<CollisionConvexHullObject>(getInnerMargin(), convexHullWithMargin, convexHullWithoutMargin);
	}

	ConvexHull3D<float> CollisionConvexHullShape::transformConvexHull(const ConvexHull3D<float> &convexHullToTransform, const PhysicsTransform &physicsTransform) const
	{
		std::map<unsigned int, Point3<float>> transformedIndexedPoints;
		for(std::map<unsigned int, Point3<float>>::const_iterator it = convexHullToTransform.getIndexedPoints().begin(); it!=convexHullToTransform.getIndexedPoints().end(); ++it)
		{
			transformedIndexedPoints[it->first] = physicsTransform.transform(it->second);
		}

		return ConvexHull3D<float>(transformedIndexedPoints, convexHullToTransform.getIndexedTriangles());
	}

	Vector3<float> CollisionConvexHullShape::computeLocalInertia(float mass) const
	{
		AABBox<float> aabbox = toAABBox(PhysicsTransform());
		float width = 2.0 * aabbox.getHalfSize(0);
		float height = 2.0 * aabbox.getHalfSize(1);
		float depth = 2.0 * aabbox.getHalfSize(2);

		float localInertia1 = (1.0/12.0) * mass * (height*height + depth*depth);
		float localInertia2 = (1.0/12.0) * mass * (width*width + depth*depth);
		float localInertia3 = (1.0/12.0) * mass * (width*width + height*height);
		return Vector3<float>(localInertia1, localInertia2, localInertia3);
	}

}
