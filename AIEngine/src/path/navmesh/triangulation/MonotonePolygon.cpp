#include <stdexcept>
#include <utility>
#include <limits>
#include <sstream>

#include "MonotonePolygon.h"

namespace urchin
{

	TypedPointCmp::TypedPointCmp(const std::vector<Point2<float>> &ccwPolygonPoints) :
		ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	bool TypedPointCmp::operator()(const TypedPoint &left, const TypedPoint &right) const
	{
		return isBelow(left.pointIndex, right.pointIndex);
	}

	bool TypedPointCmp::isBelow(unsigned int index1, unsigned int index2) const
	{
		if(ccwPolygonPoints[index1].Y == ccwPolygonPoints[index2].Y)
		{
			return ccwPolygonPoints[index1].X > ccwPolygonPoints[index2].X;
		}
		return ccwPolygonPoints[index1].Y < ccwPolygonPoints[index2].Y;
	}

	MonotonePolygon::MonotonePolygon(const std::vector<Point2<float>> &ccwPolygonPoints) :
			ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	/**
	 * Create Y-monotone polygons.
	 * Y-monotone polygon: any lines on X-axis should intersect the polygon once (point/line) or not at all.
	 */
	std::vector<std::vector<unsigned int>> MonotonePolygon::createYMonotonePolygons()
	{
		createYMonotonePolygonsDiagonals();

		std::vector<std::vector<unsigned int>> yMonotonePolygons;
		yMonotonePolygons.resize(diagonals.size()/2 + 1);

		if(diagonals.size()==0)
		{
			yMonotonePolygons[0].reserve(ccwPolygonPoints.size());
			for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
			{
				yMonotonePolygons[0].push_back(i);
			}
		}else
		{
			unsigned int polygonIndex = 0;
			for(std::multimap<unsigned int, Edge>::iterator it = diagonals.begin(); it!=diagonals.end(); ++it)
			{
				Edge startDiagonal = it->second;
				if(!startDiagonal.isProcessed)
				{
					yMonotonePolygons[polygonIndex].push_back(startDiagonal.startIndex);
					yMonotonePolygons[polygonIndex].push_back(startDiagonal.endIndex);

					unsigned int previousPointIndex = startDiagonal.startIndex;
					unsigned int currentPointIndex = startDiagonal.endIndex;

					while(true)
					{
						unsigned int nextPointIndex = retrieveNextPointIndex(previousPointIndex, currentPointIndex);
						if(nextPointIndex==startDiagonal.startIndex)
						{
							break;
						}

						yMonotonePolygons[polygonIndex].push_back(nextPointIndex);

						previousPointIndex = currentPointIndex;
						currentPointIndex = nextPointIndex;

						if(yMonotonePolygons[polygonIndex].size() > ccwPolygonPoints.size())
						{
							logImpossibleToClosePolygon();
							break;
						}
					}

					startDiagonal.isProcessed = true;
					++polygonIndex;
				}
			}
		}

		return yMonotonePolygons;
	}

	void MonotonePolygon::createYMonotonePolygonsDiagonals()
	{
		edgeHelpers.clear();
		diagonals.clear();

		bool isMonotonePolygon;
		typed_points_queue typedPointsQueue = buildTypedPointsQueue(isMonotonePolygon);
		if(isMonotonePolygon)
		{ //polygon is already monotone: no diagonal to create
			return;
		}

		edgeHelpers.reserve(5);

		while(!typedPointsQueue.empty())
		{
			PointType pointType = typedPointsQueue.top().type;
			unsigned int i = typedPointsQueue.top().pointIndex;
			switch(pointType)
			{
				case PointType::START_VERTEX:
					handleStartVertex(i);
					break;
				case PointType::SPLIT_VERTEX:
					handleSplitVertex(i);
					break;
				case PointType::END_VERTEX:
					handleEndVertex(i);
					break;
				case PointType::MERGE_VERTEX:
					handleMergeVertex(i);
					break;
				case PointType::REGULAR_DOWN_VERTEX:
					handleRegularDownVertex(i);
					break;
				case PointType::REGULAR_UP_VERTEX:
					handleRegularUpVertex(i);
					break;
				default:
					throw std::runtime_error("Unknown type of the point: " + pointType);
			}

			typedPointsQueue.pop();
		}
	}

	/**
	 * @param isMonotonePolygon [out] Returns true if polygon is already monotone
	 */
	MonotonePolygon::typed_points_queue MonotonePolygon::buildTypedPointsQueue(bool &isMonotonePolygon) const
	{
		TypedPointCmp typedPointCmp(ccwPolygonPoints);
		typed_points_queue typedPointsQueue(typedPointCmp);

		isMonotonePolygon = true;
		for(unsigned int i=0; i<ccwPolygonPoints.size(); ++i)
		{
			TypedPoint typedPoint;
			typedPoint.pointIndex = i;

			unsigned int previousIndex = (i+ccwPolygonPoints.size()-1)%ccwPolygonPoints.size();
			unsigned int nextIndex = (i+1)%ccwPolygonPoints.size();

			bool currentBelowPrevious = typedPointCmp.isBelow(i, previousIndex);
			bool currentBelowNext = typedPointCmp.isBelow(i, nextIndex);

			if(!currentBelowPrevious && !currentBelowNext)
			{
				Vector3<float> previousToOrigin = Vector3<float>(ccwPolygonPoints[previousIndex].vector(ccwPolygonPoints[i]), 0.0f);
				Vector3<float> originToNext = Vector3<float>(ccwPolygonPoints[i].vector(ccwPolygonPoints[nextIndex]), 0.0f);
				float orientationResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(previousToOrigin.crossProduct(originToNext));

				if(orientationResult>=0.0)
				{
					typedPoint.type = PointType::START_VERTEX;
				}else
				{
					typedPoint.type = PointType::SPLIT_VERTEX;
					isMonotonePolygon = false;
				}
			}else if(currentBelowPrevious && currentBelowNext)
			{
				Vector3<float> previousToOrigin = Vector3<float>(ccwPolygonPoints[previousIndex].vector(ccwPolygonPoints[i]), 0.0f);
				Vector3<float> originToNext = Vector3<float>(ccwPolygonPoints[i].vector(ccwPolygonPoints[nextIndex]), 0.0f);
				float orientationResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(previousToOrigin.crossProduct(originToNext));

				if(orientationResult>=0.0)
				{
					typedPoint.type = PointType::END_VERTEX;
				}else
				{
					typedPoint.type = PointType::MERGE_VERTEX;
					isMonotonePolygon = false;
				}
			}else
			{
				if(currentBelowPrevious && !currentBelowNext)
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

	void MonotonePolygon::handleStartVertex(unsigned int i)
	{
		createEdgeHelper(i, i, PointType::START_VERTEX);
	}

	void MonotonePolygon::handleSplitVertex(unsigned int i)
	{
		std::vector<EdgeHelper>::iterator edgeHelperIt = findNearestLeftEdgeHelper(i);

		createDiagonals(i, edgeHelperIt->helperPointIndex);

		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::SPLIT_VERTEX;

		createEdgeHelper(i, i, PointType::SPLIT_VERTEX);
	}

	void MonotonePolygon::handleEndVertex(unsigned int i)
	{
		unsigned int previousEdgeIndex = (i+ccwPolygonPoints.size()-1)%ccwPolygonPoints.size();
		std::vector<EdgeHelper>::const_iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		edgeHelpers.erase(edgeHelperIt);
	}

	void MonotonePolygon::handleMergeVertex(unsigned int i)
	{
		unsigned int previousEdgeIndex = (i+ccwPolygonPoints.size()-1)%ccwPolygonPoints.size();
		std::vector<EdgeHelper>::const_iterator edgeHelperConstIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperConstIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperConstIt->helperPointIndex);
		}
		edgeHelpers.erase(edgeHelperConstIt);

		std::vector<EdgeHelper>::iterator edgeHelperIt = findNearestLeftEdgeHelper(i);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::MERGE_VERTEX;
	}

	void MonotonePolygon::handleRegularDownVertex(unsigned int i)
	{
		unsigned int previousEdgeIndex = (i+ccwPolygonPoints.size()-1)%ccwPolygonPoints.size();
		std::vector<EdgeHelper>::const_iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		edgeHelpers.erase(edgeHelperIt);

		createEdgeHelper(i, i, PointType::REGULAR_DOWN_VERTEX);
	}

	void MonotonePolygon::handleRegularUpVertex(unsigned int i)
	{
		std::vector<EdgeHelper>::iterator edgeHelperIt = findNearestLeftEdgeHelper(i);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		edgeHelperIt->helperPointIndex = i;
		edgeHelperIt->helperPointType = PointType::REGULAR_UP_VERTEX;
	}

	void MonotonePolygon::createEdgeHelper(unsigned int edgeStartIndex, unsigned int helperPointIndex, PointType pointType)
	{
		EdgeHelper edgeHelper;
		edgeHelper.edge.startIndex = edgeStartIndex;
		edgeHelper.edge.endIndex = (edgeStartIndex+1)%ccwPolygonPoints.size();
		edgeHelper.helperPointIndex = helperPointIndex;
		edgeHelper.helperPointType = pointType;

		edgeHelpers.push_back(edgeHelper);
	}

	std::vector<EdgeHelper>::const_iterator MonotonePolygon::findEdgeHelper(unsigned int edgeIndex) const
	{
		for(std::vector<EdgeHelper>::const_iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
		{
			if(it->edge.startIndex==edgeIndex)
			{
				return it;
			}
		}

		throw std::runtime_error("Impossible to find edge and his helper for edge index: " + edgeIndex);
	}

	std::vector<EdgeHelper>::iterator MonotonePolygon::findNearestLeftEdgeHelper(unsigned int pointIndex)
	{
		Point2<float> point = ccwPolygonPoints[pointIndex];

		float minDistance = std::numeric_limits<float>::max();
		std::vector<EdgeHelper>::iterator nearestLeftEdgeHelperIt = edgeHelpers.end();

		for(std::vector<EdgeHelper>::iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
		{
			Line2D<float> edge(ccwPolygonPoints[it->edge.startIndex], ccwPolygonPoints[it->edge.endIndex]);
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

	void MonotonePolygon::createDiagonals(unsigned int index1, unsigned int index2)
	{
		Edge diagonal1;
		diagonal1.startIndex = index1;
		diagonal1.endIndex = index2;
		diagonal1.isProcessed = false;
		diagonals.insert(std::make_pair(index1, diagonal1));

		Edge diagonal2;
		diagonal2.startIndex = index2;
		diagonal2.endIndex = index1;
		diagonal2.isProcessed = false;
		diagonals.insert(std::make_pair(index2, diagonal2));
	}

	/**
	 * Returns the next point after edge [edgeStartIndex, edgeEndIndex] in order to form a monotone polygon.
	 * If edge [edgeEndIndex, nextPointIndex] is a diagonal: mark the diagonal as processed.
	 */
	unsigned int MonotonePolygon::retrieveNextPointIndex(unsigned int edgeStartIndex, unsigned int edgeEndIndex)
	{
		std::vector<std::pair<int, it_diagonals>> possibleNextPoints = retrievePossibleNextPoints(edgeEndIndex);
		if(possibleNextPoints.size()==1)
		{ //only one possible edge
			return possibleNextPoints[0].first;
		}

		int bestCCWPointIndex = -1;
		int bestCWPointIndex = -1;
		float minAngleCCW = std::numeric_limits<float>::max();
		float maxAngleCW = -std::numeric_limits<float>::max();

		Vector3<float> edgeVector = Vector3<float>(ccwPolygonPoints[edgeStartIndex].vector(ccwPolygonPoints[edgeEndIndex]), 0.0f);
		for(unsigned int i=0; i<possibleNextPoints.size(); ++i)
		{
			unsigned int testPointIndex = possibleNextPoints[i].first;
			Vector3<float> nextEdgeVector = Vector3<float>(ccwPolygonPoints[edgeEndIndex].vector(ccwPolygonPoints[testPointIndex]), 0.0f);
			float orientationResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(edgeVector.crossProduct(nextEdgeVector));
			float angle = edgeVector.normalize().dotProduct(nextEdgeVector.normalize());

			if(orientationResult > 0.0)
			{ //counter clockwise
				if(angle < minAngleCCW)
				{
					minAngleCCW = angle;
					bestCCWPointIndex = i;
				}
			}else
			{ //clockwise
				if(angle > maxAngleCW)
				{
					maxAngleCW = angle;
					bestCWPointIndex = i;
				}
			}
		}

		auto nextPoint = possibleNextPoints[bestCCWPointIndex!=-1 ? bestCCWPointIndex : bestCWPointIndex];
		markDiagonalProcessed(nextPoint.second);

		return nextPoint.first;
	}

	std::vector<std::pair<int, MonotonePolygon::it_diagonals>> MonotonePolygon::retrievePossibleNextPoints(unsigned int edgeEndIndex)
	{
		std::vector<std::pair<int, it_diagonals>> possibleNextPoints;

		unsigned int nextPolygonPointIndex = (edgeEndIndex + 1) % ccwPolygonPoints.size();
		possibleNextPoints.push_back(std::make_pair(nextPolygonPointIndex, diagonals.end()));

		auto range = diagonals.equal_range(edgeEndIndex);
		for(auto it = range.first; it != range.second; ++it)
		{
			if(!it->second.isProcessed)
			{
				possibleNextPoints.push_back(std::make_pair(it->second.endIndex, it));
			}
		}

		return possibleNextPoints;
	}

	void MonotonePolygon::markDiagonalProcessed(MonotonePolygon::it_diagonals itDiagonal)
	{
		if(itDiagonal!=diagonals.end())
		{
			itDiagonal->second.isProcessed = true;
		}
	}

	void MonotonePolygon::logImpossibleToClosePolygon() const
	{
		std::stringstream logStream;
		logStream<<"Impossible to determine monotone polygons from points:"<<std::endl;
		for(const auto &polygonPoint : ccwPolygonPoints)
		{
			logStream<<" - "<<polygonPoint<<std::endl;
		}
		Logger::logger().logError(logStream.str());
	}

}
