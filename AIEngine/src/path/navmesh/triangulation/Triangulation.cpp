#include <stdexcept>
#include <stack>
#include <algorithm>

#include "Triangulation.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin
{

	/**
	 * @param ccwPolygonPoints Polygon points in counter clockwise order. Points must be unique.
	 */
	Triangulation::Triangulation(const std::vector<Point2<float>> &ccwPolygonPoints) :
			polygonPoints(ccwPolygonPoints)
	{
		endContourIndexes.push_back(ccwPolygonPoints.size());
	}

	/**
	 * @return Polygon points in counter clockwise order.
	 */
	std::vector<Point2<float>> Triangulation::getPolygonPoints() const
	{
		return std::vector<Point2<float>>(polygonPoints.begin(), polygonPoints.begin() + endContourIndexes[0]);
	}

	/**
	 * @param cwHolePoints Hole points in clockwise order. Points must be unique and not go outside the polygon contour.
	 * @return Hole index (start to 0).
	 */
	unsigned int Triangulation::addHolePoints(const std::vector<Point2<float>> &cwHolePoints)
	{
		polygonPoints.insert(polygonPoints.end(), cwHolePoints.begin(), cwHolePoints.end());
		endContourIndexes.push_back(polygonPoints.size());

		return endContourIndexes.size() - 2;
	}

	/**
	 * @return Number of holes
	 */
	unsigned int Triangulation::getHolesSize() const
	{
		return endContourIndexes.size() - 1;
	}

	/**
	 * @return Hole points in clockwise order.
	 */
	std::vector<Point2<float>> Triangulation::getHolePoints(unsigned int holeIndex) const
	{
		return std::vector<Point2<float>>(polygonPoints.begin() + endContourIndexes[holeIndex], polygonPoints.begin() + endContourIndexes[holeIndex+1]);
	}

	std::vector<IndexedTriangle2D<float>> Triangulation::triangulate() const
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		MonotonePolygon monotonePolygon(polygonPoints, endContourIndexes);
		std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

		std::vector<IndexedTriangle2D<float>> triangles;
		triangles.reserve((polygonPoints.size()-2) + (2*getHolesSize()));

		for(unsigned monotonePolygonIndex = 0; monotonePolygonIndex<monotonePolygons.size(); ++monotonePolygonIndex)
		{
			triangulateMonotonePolygon(monotonePolygons[monotonePolygonIndex], triangles);
		}

		return triangles;
	}

	/**
	 * Return points size for all points: point of main polygon + points of holes
	 */
	unsigned int Triangulation::getAllPointsSize() const
	{
		return polygonPoints.size();
	}

	/**
	 * @param triangles [out] Triangles of monotone polygon are added to this vector
	 */
	void Triangulation::triangulateMonotonePolygon(const std::vector<unsigned int> &monotonePolygonPoints, std::vector<IndexedTriangle2D<float>> &triangles) const
	{
		std::vector<SidedPoint> sortedSidedPoints = buildSortedSidedPoints(monotonePolygonPoints);

		std::stack<SidedPoint> stack;
		stack.push(sortedSidedPoints[0]);
		stack.push(sortedSidedPoints[1]);

		for(unsigned int j=2; j<sortedSidedPoints.size()-1; ++j)
		{
			SidedPoint currentPoint = sortedSidedPoints[j];

			if(currentPoint.onLeft != stack.top().onLeft)
			{
				while(stack.size() > 1)
				{
					SidedPoint topPoint = stack.top();
					stack.pop();
					SidedPoint top2Point = stack.top();

					triangles.push_back(IndexedTriangle2D<float>(currentPoint.pointIndex, topPoint.pointIndex, top2Point.pointIndex));
				}
				stack.pop();
				stack.push(sortedSidedPoints[j-1]);
				stack.push(currentPoint);
			}else
			{
				while(stack.size() > 1)
				{
					SidedPoint topPoint = stack.top();
					stack.pop();
					SidedPoint top2Point = stack.top();
					stack.push(topPoint);

					Vector2<float> diagonalVector =polygonPoints[currentPoint.pointIndex].vector(polygonPoints[top2Point.pointIndex]);
					Vector2<float> stackVector = polygonPoints[topPoint.pointIndex].vector(polygonPoints[top2Point.pointIndex]);
					float orientationResult = diagonalVector.crossProduct(stackVector);

					if((orientationResult < 0.0 && topPoint.onLeft) || (orientationResult > 0.0 && !topPoint.onLeft))
					{
						triangles.push_back(IndexedTriangle2D<float>(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
						stack.pop();
					}else
					{
						break;
					}
				}

				stack.push(currentPoint);
			}
		}

		SidedPoint currentPoint = sortedSidedPoints[sortedSidedPoints.size()-1];
		while(stack.size() > 1)
		{
			SidedPoint topPoint = stack.top();
			stack.pop();
			SidedPoint top2Point = stack.top();

			triangles.push_back(IndexedTriangle2D<float>(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
		}
	}

	std::vector<SidedPoint> Triangulation::buildSortedSidedPoints(const std::vector<unsigned int> &monotonePolygonPoints) const
	{
		std::vector<SidedPoint> sortedSidedPoints;
		sortedSidedPoints.reserve(monotonePolygonPoints.size());

		for(unsigned int i=0, previousI=monotonePolygonPoints.size()-1; i<monotonePolygonPoints.size(); previousI=i++)
		{
			SidedPoint sidedPoint;

			unsigned int currentIndex = monotonePolygonPoints[i];
			sidedPoint.pointIndex = currentIndex;
			sidedPoint.onLeft = isFirstPointAboveSecond(monotonePolygonPoints[previousI], currentIndex);

			sortedSidedPoints.push_back(sidedPoint);
		}

		std::sort(sortedSidedPoints.begin(), sortedSidedPoints.end(), [&](const SidedPoint &left, const SidedPoint &right)
				{return isFirstPointAboveSecond(left.pointIndex, right.pointIndex);});

		return sortedSidedPoints;
	}

	bool Triangulation::isFirstPointAboveSecond(unsigned int firstIndex, unsigned int secondIndex) const
	{
		if(polygonPoints[firstIndex].Y == polygonPoints[secondIndex].Y)
		{
			return polygonPoints[firstIndex].X < polygonPoints[secondIndex].X;
		}
		return polygonPoints[firstIndex].Y > polygonPoints[secondIndex].Y;
	}
}
