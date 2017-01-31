#include <stdexcept>

#include "Triangulation.h"

namespace urchin
{

	TypedPointCmp::TypedPointCmp(const std::vector<Point2<float>> &ccwPolygonPoints) :
		ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	bool TypedPointCmp::operator()(const TypedPoint &left, const TypedPoint &right) const
	{
		if(ccwPolygonPoints[left.pointIndex].Y == ccwPolygonPoints[right.pointIndex].Y)
		{
			return ccwPolygonPoints[left.pointIndex].X < ccwPolygonPoints[right.pointIndex].X;
		}
		return ccwPolygonPoints[left.pointIndex].Y < ccwPolygonPoints[right.pointIndex].Y;
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
		std::vector<EdgeHelper> edgeHelpers; //TODO useful to replace by binary tree ?
		edgeHelpers.reserve(5);

		while(!typedPointsQueue.empty())
		{
			PointType pointType = typedPointsQueue.top().type;
			unsigned int i = typedPointsQueue.top().pointIndex;
			switch(pointType)
			{
				case PointType::START_VERTEX:
					handleStartVertex(i, edgeHelpers);
					break;
				case PointType::SPLIT_VERTEX:
					handleSplitVertex(i, edgeHelpers);
					break;
				case PointType::END_VERTEX:
					handleEndVertex(i, edgeHelpers);
					break;
				case PointType::MERGE_VERTEX:
					handleMergeVertex(i, edgeHelpers);
					break;
				case PointType::REGULAR_DOWN_VERTEX:
					handleRegularDownVertex(i, edgeHelpers);
					break;
				case PointType::REGULAR_UP_VERTEX:
					handleRegularUpVertex(i, edgeHelpers);
					break;
				default:
					throw std::runtime_error("Unknown type of the point: " + pointType);
			}

			typedPointsQueue.pop();
		}
	}

	typed_points_queue Triangulation::buildTypedPointsQueue() const
	{
		TypedPointCmp typedPointCmp(ccwPolygonPoints);
		typed_points_queue typedPointsQueue(typedPointCmp);

		for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
		{
			TypedPoint typedPoint;
			typedPoint.pointIndex = i;

			unsigned int previousIndex = (i-1)%ccwPolygonPoints.size();
			unsigned int nextIndex = (i+1)%ccwPolygonPoints.size();

			if(ccwPolygonPoints[i].Y > ccwPolygonPoints[previousIndex].Y && ccwPolygonPoints[i].Y > ccwPolygonPoints[nextIndex].Y)
			{
				Vector3<float> previousToOrigin = Vector3<float>(ccwPolygonPoints[previousIndex].vector(ccwPolygonPoints[i]), 0.0f);
				Vector3<float> originToNext = Vector3<float>(ccwPolygonPoints[i].vector(ccwPolygonPoints[nextIndex]), 0.0f);
				float dotResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(previousToOrigin.crossProduct(originToNext));

				typedPoint.type = dotResult>=0.0 ? PointType::START_VERTEX : PointType::SPLIT_VERTEX;
			}else if(ccwPolygonPoints[i].Y < ccwPolygonPoints[previousIndex].Y && ccwPolygonPoints[i].Y < ccwPolygonPoints[nextIndex].Y)
			{
				Vector3<float> previousToOrigin = Vector3<float>(ccwPolygonPoints[previousIndex].vector(ccwPolygonPoints[i]), 0.0f);
				Vector3<float> originToNext = Vector3<float>(ccwPolygonPoints[i].vector(ccwPolygonPoints[nextIndex]), 0.0f);
				float dotResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(previousToOrigin.crossProduct(originToNext));

				typedPoint.type = dotResult>=0.0 ? PointType::END_VERTEX : PointType::MERGE_VERTEX;
			}else
			{
				if(ccwPolygonPoints[i].Y < ccwPolygonPoints[previousIndex].Y && ccwPolygonPoints[i].Y > ccwPolygonPoints[nextIndex].Y)
				{
					typedPoint.type = PointType::REGULAR_DOWN_VERTEX;
				}else
				{
					typedPoint.type = PointType::REGULAR_UP_VERTEX;
				}
			}

			typedPointsQueue.push(typedPoint);
		}

		return typedPointsQueue;
	}

	void Triangulation::handleStartVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		EdgeHelper edgeHelper;
		edgeHelper.edgeStartIndex = i;
		edgeHelper.helperPointIndex = i;
		edgeHelper.helperPointType = PointType::START_VERTEX;

		edgeHelpers.push_back(edgeHelper);
	}

	void Triangulation::handleSplitVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		std::vector<EdgeHelper>::iterator edgeHelperIt = findNearestLeftEdgeHelper(i, edgeHelpers);

		std::cout<<"Connect split: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO

		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::SPLIT_VERTEX;

		EdgeHelper edgeHelper;
		edgeHelper.edgeStartIndex = i;
		edgeHelper.helperPointIndex = i;
		edgeHelper.helperPointType = PointType::SPLIT_VERTEX;
		edgeHelpers.push_back(edgeHelper);
	}

	void Triangulation::handleEndVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		unsigned int previousEdgeIndex = i-1;
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex, edgeHelpers);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			std::cout<<"Connect end: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO
		}
		edgeHelpers.erase(edgeHelperIt);
	}

	void Triangulation::handleMergeVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		unsigned int previousEdgeIndex = i-1;
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex, edgeHelpers);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			std::cout<<"Connect merge 1: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO
		}
		edgeHelpers.erase(edgeHelperIt);

		edgeHelperIt = findNearestLeftEdgeHelper(i, edgeHelpers);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			std::cout<<"Connect merge 2: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO
		}
		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::MERGE_VERTEX;
	}

	void Triangulation::handleRegularDownVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		unsigned int previousEdgeIndex = i-1;
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex, edgeHelpers);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			std::cout<<"Connect reg. down: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO
		}
		edgeHelpers.erase(edgeHelperIt);

		EdgeHelper edgeHelper;
		edgeHelper.edgeStartIndex = i;
		edgeHelper.helperPointIndex = i;
		edgeHelper.helperPointType = PointType::REGULAR_DOWN_VERTEX;
		edgeHelpers.push_back(edgeHelper);
	}

	void Triangulation::handleRegularUpVertex(unsigned int i, std::vector<EdgeHelper> &edgeHelpers) const
	{
		std::vector<EdgeHelper>::iterator edgeHelperIt = findNearestLeftEdgeHelper(i, edgeHelpers);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			std::cout<<"Connect reg. up: "<<i<<" and "<<edgeHelperIt->helperPointIndex<<std::endl; //TODO
		}
		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::REGULAR_UP_VERTEX;
	}

	std::vector<EdgeHelper>::iterator Triangulation::findEdgeHelper(unsigned int edgeIndex, std::vector<EdgeHelper> &edgeHelpers) const
	{
		for(std::vector<EdgeHelper>::iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
		{
			if(it->edgeStartIndex==edgeIndex)
			{
				return it;
			}
		}

		throw std::runtime_error("Impossible to find edge and his helper for edge index: " + edgeIndex);
	}

	std::vector<EdgeHelper>::iterator Triangulation::findNearestLeftEdgeHelper(unsigned int pointIndex, std::vector<EdgeHelper> &edgeHelpers) const
	{
		Point2<float> point = ccwPolygonPoints[pointIndex];

		float minDistance = std::numeric_limits<float>::max();
		std::vector<EdgeHelper>::iterator nearestLeftEdgeHelperIt = edgeHelpers.end();

		for(std::vector<EdgeHelper>::iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
		{
			Line2D<float> edge(ccwPolygonPoints[it->edgeStartIndex], ccwPolygonPoints[(it->edgeStartIndex+1)%ccwPolygonPoints.size()]);
			Point2<float> nearestPointOnEdge = edge.orthogonalProjection(point);
			if(nearestPointOnEdge.X  < point.X)
			{ //edge is on left of point
				float edgePointDistance = nearestPointOnEdge.squareDistance(point);
				if(edgePointDistance < minDistance)
				{
					minDistance = edgePointDistance;
					nearestLeftEdgeHelperIt = it;
				}
			}
		}

		if(nearestLeftEdgeHelperIt==edgeHelpers.end())
		{
			throw std::runtime_error("Impossible to find edge on left for point index: " + std::to_string(pointIndex));
		}

		return nearestLeftEdgeHelperIt;
	}

}
