#include <stdexcept>
#include <stack>
#include <algorithm>

#include "Triangulation.h"
#include "path/navmesh/triangulation/MonotonePolygon.h"

namespace urchin
{

	SidedPointCmp::SidedPointCmp(const std::vector<Point2<float>> &ccwPolygonPoints) :
		ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	bool SidedPointCmp::operator()(const SidedPoint &left, const SidedPoint &right) const
	{
		return isAbove(left.pointIndex, right.pointIndex);
	}

	bool SidedPointCmp::isAbove(unsigned int index1, unsigned int index2) const
	{
		if(ccwPolygonPoints[index1].Y == ccwPolygonPoints[index2].Y)
		{
			return ccwPolygonPoints[index1].X < ccwPolygonPoints[index2].X;
		}
		return ccwPolygonPoints[index1].Y > ccwPolygonPoints[index2].Y;
	}

	Triangulation::Triangulation(const std::vector<Point2<float>> &ccwPolygonPoints) :
				ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	std::vector<IndexedTriangle2D<float>> Triangulation::triangulate() const
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		MonotonePolygon monotonePolygon(ccwPolygonPoints);
		std::vector<std::vector<unsigned int>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

		std::vector<IndexedTriangle2D<float>> triangles;
		triangles.reserve(ccwPolygonPoints.size());

		for(unsigned monotonePolygonIndex = 0; monotonePolygonIndex<monotonePolygons.size(); ++monotonePolygonIndex)
		{
			triangulateMonotonePolygon(monotonePolygons[monotonePolygonIndex], triangles);
		}

		return triangles;
	}

	/**
	 * @param triangles [out] Triangles of monotone polygon are added to this vector
	 */
	void Triangulation::triangulateMonotonePolygon(const std::vector<unsigned int> &monotonePolygonPoints, std::vector<IndexedTriangle2D<float>> &triangles) const
	{
		std::vector<SidedPoint> sidedPointsVector = buildSidedPointsVector(monotonePolygonPoints);

		std::stack<SidedPoint> stack;
		stack.push(sidedPointsVector[0]);
		stack.push(sidedPointsVector[1]);

		for(unsigned int j=2; j<sidedPointsVector.size()-1; ++j)
		{
			SidedPoint currentPoint = sidedPointsVector[j];

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
				stack.push(sidedPointsVector[j-1]);
				stack.push(currentPoint);
			}else
			{
				while(stack.size() > 1)
				{
					SidedPoint topPoint = stack.top();
					stack.pop();
					SidedPoint top2Point = stack.top();
					stack.push(topPoint);

					Vector3<float> diagonalVector = Vector3<float>(ccwPolygonPoints[currentPoint.pointIndex].vector(ccwPolygonPoints[top2Point.pointIndex]), 0.0f);
					Vector3<float> stackVector = Vector3<float>(ccwPolygonPoints[topPoint.pointIndex].vector(ccwPolygonPoints[top2Point.pointIndex]), 0.0f);
					float orientationResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(diagonalVector.crossProduct(stackVector));

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

		SidedPoint currentPoint = sidedPointsVector[sidedPointsVector.size()-1];
		while(stack.size() > 1)
		{
			SidedPoint topPoint = stack.top();
			stack.pop();
			SidedPoint top2Point = stack.top();

			triangles.push_back(IndexedTriangle2D<float>(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
		}
	}

	std::vector<SidedPoint> Triangulation::buildSidedPointsVector(const std::vector<unsigned int> &monotonePolygonPoints) const
	{
		SidedPointCmp sidedPointCmp(ccwPolygonPoints);

		std::vector<SidedPoint> sidedPointsVector;
		sidedPointsVector.reserve(monotonePolygonPoints.size());

		for(unsigned int i=0; i<monotonePolygonPoints.size(); ++i)
		{
			SidedPoint sidedPoint;

			unsigned int currentIndex = monotonePolygonPoints[i];
			sidedPoint.pointIndex = currentIndex;

			unsigned int nextIndex = monotonePolygonPoints[(i+1)%monotonePolygonPoints.size()];
			sidedPoint.onLeft = sidedPointCmp.isAbove(currentIndex, nextIndex);

			sidedPointsVector.push_back(sidedPoint);
		}

		std::sort(sidedPointsVector.begin(), sidedPointsVector.end(), sidedPointCmp);

		return sidedPointsVector;
	}
}
