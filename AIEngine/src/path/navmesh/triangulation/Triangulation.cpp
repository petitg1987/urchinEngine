#include "Triangulation.h"

namespace urchin
{

	bool TypedPoint::operator()(const TypedPoint &left, const TypedPoint &right) const
	{
		if(left.point.Y == right.point.Y)
		{
			return left.point.X < right.point.X;
		}
		return left.point.Y < right.point.Y;
	}

	Triangulation::Triangulation(const std::vector<Point2<float>> &ccwPolygonPoints) :
				ccwPolygonPoints(ccwPolygonPoints)
	{

	}


	void Triangulation::triangulate() const
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		createYMonotonePolygons();
	}

	/**
	 * Create Y-monotone polygons.
	 * Y-monotone polygon: any lines on X-axis should intersect the polygon once (point/line) or not at all.
	 */
	void Triangulation::createYMonotonePolygons() const
	{
		typed_points_queue typedPointsQueue = buildTypedPointsQueue();
		while(!typedPointsQueue.empty())
		{
			std::cout<<"Point: "<<typedPointsQueue.top().point<<", Type: "<<typedPointsQueue.top().type<<std::endl;
			typedPointsQueue.pop();
		}
	}

	typed_points_queue Triangulation::buildTypedPointsQueue() const
	{
		typed_points_queue typedPointsQueue;

		for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
		{
			TypedPoint typedPoint;
			typedPoint.point = ccwPolygonPoints[i];

			unsigned int previousIndex = (i-1)%ccwPolygonPoints.size();
			unsigned int nextIndex = (i+1)%ccwPolygonPoints.size();

			if(ccwPolygonPoints[i].Y > ccwPolygonPoints[previousIndex].Y && ccwPolygonPoints[i].Y > ccwPolygonPoints[nextIndex].Y)
			{
				Vector2<float> originToPrevious = ccwPolygonPoints[i].vector(ccwPolygonPoints[previousIndex]);
				Vector2<float> originToPreviousNormal = Vector2<float>(-originToPrevious.Y, originToPrevious.X).normalize();

				Vector2<float> upVector = Vector2<float>(0.0, 1.0);
				typedPoint.type = (originToPreviousNormal.dotProduct(upVector) >= 0.0) ? PointType::START_VERTEX : PointType::SPLIT_VERTEX;
			}else if(ccwPolygonPoints[i].Y < ccwPolygonPoints[previousIndex].Y && ccwPolygonPoints[i].Y < ccwPolygonPoints[nextIndex].Y)
			{
				Vector2<float> originToPrevious = ccwPolygonPoints[i].vector(ccwPolygonPoints[previousIndex]);
				Vector2<float> originToPreviousNormal = Vector2<float>(-originToPrevious.Y, originToPrevious.X).normalize();

				Vector2<float> downVector = Vector2<float>(0.0, -1.0);
				typedPoint.type = (originToPreviousNormal.dotProduct(downVector) >= 0.0) ? PointType::END_VERTEX : PointType::MERGE_VERTEX;
			}else
			{
				typedPoint.type = PointType::REGULAR_VERTEX;
			}

			typedPointsQueue.push(typedPoint);
		}

		return typedPointsQueue;
	}

}
