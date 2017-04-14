#include <stdexcept>
#include <utility>
#include <algorithm>
#include <limits>
#include <sstream>

#include "MonotonePolygon.h"

namespace urchin
{

	/**
	 * @param polygonPoints Polygon points are in CCW order and holes in CW order.
	 * @param endContourIndexes Delimiter between polygon points and holes points.
	 */
	MonotonePolygon::MonotonePolygon(const std::vector<Point2<float>> &polygonPoints, const std::vector<unsigned int> &endContourIndexes) :
			polygonPoints(polygonPoints),
			endContourIndexes(endContourIndexes)
	{
		edgeHelpers.reserve(5);
	}

	/**
	 * Create Y-monotone polygons.
	 * Y-monotone polygon: any lines on X-axis should intersect the polygon once (point/line) or not at all.
	 */
	std::vector<std::vector<unsigned int>> MonotonePolygon::createYMonotonePolygons()
	{
		createYMonotonePolygonsDiagonals();

		std::vector<std::vector<unsigned int>> yMonotonePolygons;
		yMonotonePolygons.reserve(diagonals.size());

		if(diagonals.size()==0)
		{
			std::vector<unsigned int> monotonePointsIndexes;
			monotonePointsIndexes.reserve(polygonPoints.size());
			for(unsigned int i=0; i<polygonPoints.size(); ++i)
			{
				monotonePointsIndexes.push_back(i);
			}
			yMonotonePolygons.push_back(monotonePointsIndexes);
		}else
		{
			for(std::multimap<unsigned int, Edge>::iterator it = diagonals.begin(); it!=diagonals.end(); ++it)
			{
				Edge startDiagonal = it->second;
				if(!startDiagonal.isProcessed)
				{
					std::vector<unsigned int> monotonePointsIndexes;
					monotonePointsIndexes.reserve(polygonPoints.size()/2 + 1); //estimated memory size
					monotonePointsIndexes.push_back(startDiagonal.startIndex);
					monotonePointsIndexes.push_back(startDiagonal.endIndex);

					unsigned int previousPointIndex = startDiagonal.startIndex;
					unsigned int currentPointIndex = startDiagonal.endIndex;

					while(true)
					{
						unsigned int nextPointIndex = retrieveNextPointIndex(previousPointIndex, currentPointIndex);
						if(nextPointIndex==startDiagonal.startIndex)
						{
							break;
						}

						monotonePointsIndexes.push_back(nextPointIndex);

						previousPointIndex = currentPointIndex;
						currentPointIndex = nextPointIndex;

						if(monotonePointsIndexes.size() > polygonPoints.size())
						{
							logImpossibleToClosePolygon();
							break;
						}
					}

					startDiagonal.isProcessed = true;
					yMonotonePolygons.push_back(monotonePointsIndexes);
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
		std::vector<TypedPoint> sortedTypedPoints = buildSortedTypedPoints(isMonotonePolygon);
		if(isMonotonePolygon)
		{ //polygon is already monotone: no diagonal to create
			return;
		}

		for(const auto &typedPoint : sortedTypedPoints)
		{
			switch(typedPoint.type)
			{
				case PointType::START_VERTEX:
					handleStartVertex(typedPoint.pointIndex);
					break;
				case PointType::SPLIT_VERTEX:
					handleSplitVertex(typedPoint.pointIndex);
					break;
				case PointType::END_VERTEX:
					handleEndVertex(typedPoint.pointIndex);
					break;
				case PointType::MERGE_VERTEX:
					handleMergeVertex(typedPoint.pointIndex);
					break;
				case PointType::REGULAR_DOWN_VERTEX:
					handleRegularDownVertex(typedPoint.pointIndex);
					break;
				case PointType::REGULAR_UP_VERTEX:
					handleRegularUpVertex(typedPoint.pointIndex);
					break;
				default:
					throw std::runtime_error("Unknown type of the point: " + typedPoint.type);
			}
		}
	}

	/**
	 * @param isMonotonePolygon [out] Returns true if polygon is already monotone
	 */
	std::vector<TypedPoint> MonotonePolygon::buildSortedTypedPoints(bool &isMonotonePolygon) const
	{
		std::vector<TypedPoint> sortedTypedPoints;
		sortedTypedPoints.reserve(polygonPoints.size());

		isMonotonePolygon = true;
		for(unsigned int i=0; i<polygonPoints.size(); ++i)
		{
			TypedPoint typedPoint;
			typedPoint.pointIndex = i;

			unsigned int previousIndex = previousPointIndex(i);
			unsigned int nextIndex = nextPointIndex(i);

			bool currentAbovePrevious = isFirstPointAboveSecond(i, previousIndex);
			bool currentAboveNext = isFirstPointAboveSecond(i, nextIndex);

			if(currentAbovePrevious && currentAboveNext)
			{
				Vector2<float> previousToOrigin = polygonPoints[previousIndex].vector(polygonPoints[i]);
				Vector2<float> originToNext = polygonPoints[i].vector(polygonPoints[nextIndex]);
				float orientationResult = previousToOrigin.crossProduct(originToNext);

				if(orientationResult>=0.0)
				{
					typedPoint.type = PointType::START_VERTEX;
				}else
				{
					typedPoint.type = PointType::SPLIT_VERTEX;
					isMonotonePolygon = false;
				}
			}else if(!currentAbovePrevious && !currentAboveNext)
			{
				Vector2<float> previousToOrigin = polygonPoints[previousIndex].vector(polygonPoints[i]);
				Vector2<float> originToNext = polygonPoints[i].vector(polygonPoints[nextIndex]);
				float orientationResult = previousToOrigin.crossProduct(originToNext);

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
				if(!currentAbovePrevious && currentAboveNext)
				{
					typedPoint.type = PointType::REGULAR_DOWN_VERTEX;
				}else
				{
					typedPoint.type = PointType::REGULAR_UP_VERTEX;
				}
			}

			sortedTypedPoints.push_back(typedPoint);
		}

		std::sort(sortedTypedPoints.begin(), sortedTypedPoints.end(), [&](const TypedPoint &left, const TypedPoint &right)
						{return isFirstPointAboveSecond(left.pointIndex, right.pointIndex);});

		if(!sortedTypedPoints.empty() && sortedTypedPoints[0].type!=PointType::START_VERTEX)
		{
			throw std::runtime_error("First point in the vector should be a start vertex. Point type: " + sortedTypedPoints[0].type);
		}

		return sortedTypedPoints;
	}

	bool MonotonePolygon::isFirstPointAboveSecond(unsigned int firstIndex, unsigned int secondIndex) const
	{
		if(polygonPoints[firstIndex].Y == polygonPoints[secondIndex].Y)
		{
			return polygonPoints[firstIndex].X < polygonPoints[secondIndex].X;
		}
		return polygonPoints[firstIndex].Y > polygonPoints[secondIndex].Y;
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
		unsigned int previousEdgeIndex = previousPointIndex(i);
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		VectorEraser::erase(edgeHelpers, edgeHelperIt);
	}

	void MonotonePolygon::handleMergeVertex(unsigned int i)
	{
		unsigned int previousEdgeIndex = previousPointIndex(i);
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		VectorEraser::erase(edgeHelpers, edgeHelperIt);

		std::vector<EdgeHelper>::iterator nearestLeftEdgeHelperIt = findNearestLeftEdgeHelper(i);
		if(nearestLeftEdgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, nearestLeftEdgeHelperIt->helperPointIndex);
		}
		nearestLeftEdgeHelperIt->helperPointIndex = i;
		nearestLeftEdgeHelperIt->helperPointType = PointType::MERGE_VERTEX;
	}

	void MonotonePolygon::handleRegularDownVertex(unsigned int i)
	{
		unsigned int previousEdgeIndex = previousPointIndex(i);
		std::vector<EdgeHelper>::iterator edgeHelperIt = findEdgeHelper(previousEdgeIndex);
		if(edgeHelperIt->helperPointType==PointType::MERGE_VERTEX)
		{
			createDiagonals(i, edgeHelperIt->helperPointIndex);
		}
		VectorEraser::erase(edgeHelpers, edgeHelperIt);

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

	unsigned int MonotonePolygon::nextPointIndex(unsigned int pointIndex) const
	{
		unsigned int nextPointIndex = pointIndex + 1;

		auto it = std::find(endContourIndexes.begin(), endContourIndexes.end(), nextPointIndex);
		if(it==endContourIndexes.begin())
		{
			nextPointIndex = 0;
		}else if(it!=endContourIndexes.end())
		{
			nextPointIndex = *(--it);
		}

		return nextPointIndex;
	}

	unsigned int MonotonePolygon::previousPointIndex(unsigned int pointIndex) const
	{
		unsigned int previousPointIndex = pointIndex - 1;
		if(pointIndex==0)
		{
			previousPointIndex = endContourIndexes[0] - 1;
		}else
		{
			auto it = std::find(endContourIndexes.begin(), endContourIndexes.end(), pointIndex);
			if(it!=endContourIndexes.end())
			{
				previousPointIndex = (*(++it)) - 1;
			}
		}

		return previousPointIndex;
	}

	void MonotonePolygon::createEdgeHelper(unsigned int edgeStartIndex, unsigned int helperPointIndex, PointType pointType)
	{
		EdgeHelper edgeHelper;
		edgeHelper.edge.startIndex = edgeStartIndex;
		edgeHelper.edge.endIndex = nextPointIndex(edgeStartIndex);
		edgeHelper.helperPointIndex = helperPointIndex;
		edgeHelper.helperPointType = pointType;

		edgeHelpers.push_back(edgeHelper);
	}

	std::vector<EdgeHelper>::iterator MonotonePolygon::findEdgeHelper(unsigned int edgeIndex)
	{
		for(std::vector<EdgeHelper>::iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
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
		Point2<float> point = polygonPoints[pointIndex];

		float minDistance = std::numeric_limits<float>::max();
		std::vector<EdgeHelper>::iterator nearestLeftEdgeHelperIt = edgeHelpers.end();

		for(std::vector<EdgeHelper>::iterator it=edgeHelpers.begin(); it!=edgeHelpers.end(); ++it)
		{
			Line2D<float> edge(polygonPoints[it->edge.startIndex], polygonPoints[it->edge.endIndex]);
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

		Vector2<float> edgeVector = polygonPoints[edgeStartIndex].vector(polygonPoints[edgeEndIndex]);
		for(unsigned int i=0; i<possibleNextPoints.size(); ++i)
		{
			unsigned int testPointIndex = possibleNextPoints[i].first;
			Vector2<float> nextEdgeVector = polygonPoints[edgeEndIndex].vector(polygonPoints[testPointIndex]);
			float orientationResult = edgeVector.crossProduct(nextEdgeVector);
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
		possibleNextPoints.reserve(diagonals.size());

		unsigned int nextPolygonPointIndex = nextPointIndex(edgeEndIndex);
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
		for(unsigned int i=0; i<polygonPoints.size(); ++i)
		{
			logStream<<" - "<<polygonPoints[i]<<std::endl;

			if((i+1)!=polygonPoints.size() && std::find(endContourIndexes.begin(), endContourIndexes.end(), i+1) != endContourIndexes.end())
			{
				logStream<<" - Hole:"<<std::endl;
			}
		}
		Logger::logger().logError(logStream.str());
	}

}
