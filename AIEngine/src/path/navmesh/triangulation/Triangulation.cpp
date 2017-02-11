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
		if(ccwPolygonPoints[left.pointIndex].Y == ccwPolygonPoints[right.pointIndex].Y)
		{
			return ccwPolygonPoints[left.pointIndex].X < ccwPolygonPoints[right.pointIndex].X;
		}
		return ccwPolygonPoints[left.pointIndex].Y > ccwPolygonPoints[right.pointIndex].Y;
	}

	Triangulation::Triangulation(const std::vector<Point2<float>> &ccwPolygonPoints) :
				ccwPolygonPoints(ccwPolygonPoints)
	{

	}

	void Triangulation::triangulate() const //TODO return vector of IndexedTriangle...
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		MonotonePolygon monotonePolygon(ccwPolygonPoints);
		std::vector<std::vector<Point2<float>>> monotonePolygons = monotonePolygon.createYMonotonePolygons();

		for(unsigned monotonePolygonIndex = 0; monotonePolygonIndex<monotonePolygons.size(); ++monotonePolygonIndex)
		{
			triangulateMonotonePolygon(monotonePolygons[monotonePolygonIndex]);
		}
	}

	void Triangulation::triangulateMonotonePolygon(const std::vector<Point2<float>> &monotonePolygonPoints) const
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

					std::cout<<"Create triangle A: "<<currentPoint.pointIndex<<", "<<topPoint.pointIndex<<", "<<top2Point.pointIndex<<std::endl;
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

					Vector3<float> diagonalVector = Vector3<float>(monotonePolygonPoints[currentPoint.pointIndex].vector(monotonePolygonPoints[top2Point.pointIndex]), 0.0f);
					Vector3<float> stackVector = Vector3<float>(monotonePolygonPoints[topPoint.pointIndex].vector(monotonePolygonPoints[top2Point.pointIndex]), 0.0f);
					float orientationResult = Vector3<float>(0.0, 0.0, 1.0).dotProduct(diagonalVector.crossProduct(stackVector));

					if((orientationResult < 0.0 && topPoint.onLeft) || (orientationResult > 0.0 && !topPoint.onLeft)) {
						std::cout<<"Create triangle B: "<<currentPoint.pointIndex<<", "<<top2Point.pointIndex<<", "<<topPoint.pointIndex<<std::endl;
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

			std::cout<<"Create triangle C: "<<currentPoint.pointIndex<<", "<<top2Point.pointIndex<<", "<<topPoint.pointIndex<<std::endl;
		}
	}

	std::vector<SidedPoint> Triangulation::buildSidedPointsVector(const std::vector<Point2<float>> &monotonePolygonPoints) const
	{
		SidedPointCmp sidedPointCmp(monotonePolygonPoints);

		std::vector<SidedPoint> sidedPointsVector;
		sidedPointsVector.reserve(monotonePolygonPoints.size());

		for(unsigned int i=0; i<monotonePolygonPoints.size(); ++i)
		{
			SidedPoint sidedPoint;
			sidedPoint.pointIndex = i;

			unsigned int nextIndex = (i+1)%monotonePolygonPoints.size();
			sidedPoint.onLeft = (monotonePolygonPoints[i].Y > monotonePolygonPoints[nextIndex].Y)
										|| (monotonePolygonPoints[i].Y == monotonePolygonPoints[nextIndex].Y && monotonePolygonPoints[i].X < monotonePolygonPoints[nextIndex].X); //TODO use SidedPointCmp

			sidedPointsVector.push_back(sidedPoint);
		}

		std::sort(sidedPointsVector.begin(), sidedPointsVector.end(), sidedPointCmp);

		return sidedPointsVector;
	}
}
