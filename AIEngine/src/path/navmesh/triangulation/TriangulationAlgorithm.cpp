#include <stdexcept>
#include <stack>
#include <algorithm>

#include "TriangulationAlgorithm.h"
#include "path/navmesh/triangulation/MonotonePolygonAlgorithm.h"

namespace urchin
{

	SidedPoint::SidedPoint(unsigned int pointIndex, bool onLeft) :
			pointIndex(pointIndex), onLeft(onLeft)
	{

	}

	/**
	 * @param ccwPolygonPoints Polygon points in counter clockwise order. Points must be unique.
	 */
	TriangulationAlgorithm::TriangulationAlgorithm(const std::vector<Point2<float>> &ccwPolygonPoints) :
			polygonPoints(ccwPolygonPoints)
	{
		endContourIndices.push_back(ccwPolygonPoints.size());
	}

	/**
	 * @return Polygon points in counter clockwise order.
	 */
	std::vector<Point2<float>> TriangulationAlgorithm::getPolygonPoints() const
	{
		return std::vector<Point2<float>>(polygonPoints.begin(), polygonPoints.begin() + endContourIndices[0]);
	}

	/**
	 * @param cwHolePoints Hole points in clockwise order. Points must be unique and not go outside the polygon contour.
	 * @return Hole index (start to 0).
	 */
	unsigned int TriangulationAlgorithm::addHolePoints(const std::vector<Point2<float>> &cwHolePoints)
	{
		polygonPoints.insert(polygonPoints.end(), cwHolePoints.begin(), cwHolePoints.end());
		endContourIndices.push_back(polygonPoints.size());

		return endContourIndices.size() - 2;
	}

	/**
	 * @return Number of holes
	 */
	unsigned int TriangulationAlgorithm::getHolesSize() const
	{
		return endContourIndices.size() - 1;
	}

	/**
	 * @return Hole points in clockwise order.
	 */
	std::vector<Point2<float>> TriangulationAlgorithm::getHolePoints(unsigned int holeIndex) const
	{
		return std::vector<Point2<float>>(polygonPoints.begin() + endContourIndices[holeIndex], polygonPoints.begin() + endContourIndices[holeIndex+1]);
	}

	std::vector<IndexedTriangleMesh> TriangulationAlgorithm::triangulate() const
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		std::vector<MonotonePolygon> monotonePolygons = MonotonePolygonAlgorithm(polygonPoints, endContourIndices).createYMonotonePolygons();

		std::vector<IndexedTriangleMesh> triangles;
		triangles.reserve((polygonPoints.size()-2) + (2*getHolesSize()));

		for (const auto &monotonePolygon : monotonePolygons)
		{
			#ifdef _DEBUG
				std::vector<IndexedTriangleMesh> monotonePolygonTriangles;
				monotonePolygonTriangles.reserve(monotonePolygon.getCcwPoints().size());
				triangulateMonotonePolygon(monotonePolygon.getCcwPoints(), monotonePolygonTriangles);
				triangles.insert(triangles.end(), monotonePolygonTriangles.begin(), monotonePolygonTriangles.end());
				//logOutputData("Debug monotone polygon " + std::to_string(monotonePolygonIndex) + " triangulation.", monotonePolygonTriangles, Logger::INFO);
			#else
				triangulateMonotonePolygon(monotonePolygon.getCcwPoints(), triangles);
			#endif
		}

		return triangles;
	}

	/**
	 * Return points size for all points: point of main polygon + points of holes
	 */
	unsigned int TriangulationAlgorithm::getAllPointsSize() const
	{
		return polygonPoints.size();
	}

	/**
	 * @param triangles [out] Triangles of monotone polygon are added to this vector
	 */
	void TriangulationAlgorithm::triangulateMonotonePolygon(const std::vector<unsigned int> &monotonePolygonPoints, std::vector<IndexedTriangleMesh> &triangles) const
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

					triangles.emplace_back(IndexedTriangleMesh(currentPoint.pointIndex, topPoint.pointIndex, top2Point.pointIndex));
					determineNeighbor(triangles);
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

					Vector2<float> diagonalVector = polygonPoints[currentPoint.pointIndex].vector(polygonPoints[top2Point.pointIndex]);
					Vector2<float> stackVector = polygonPoints[topPoint.pointIndex].vector(polygonPoints[top2Point.pointIndex]);
					float orientationResult = diagonalVector.crossProduct(stackVector); //note: orientation could be 0.0 if currentPoint.pointIndex and topPoint.pointIndex are very close due to float imprecision

					if((orientationResult <= 0.0 && topPoint.onLeft) || (orientationResult >= 0.0 && !topPoint.onLeft))
					{
						triangles.emplace_back(IndexedTriangleMesh(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
						determineNeighbor(triangles);
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

			triangles.emplace_back(IndexedTriangleMesh(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
			determineNeighbor(triangles);
		}
	}

	std::vector<SidedPoint> TriangulationAlgorithm::buildSortedSidedPoints(const std::vector<unsigned int> &monotonePolygonPoints) const
	{
		std::vector<SidedPoint> sortedSidedPoints;
		sortedSidedPoints.reserve(monotonePolygonPoints.size());

		for(unsigned int i=0; i<monotonePolygonPoints.size(); ++i)
		{
			unsigned int currentIndex = monotonePolygonPoints[i];
			unsigned int nextIndex = monotonePolygonPoints[(i+1)%monotonePolygonPoints.size()];

			sortedSidedPoints.emplace_back(SidedPoint(currentIndex, isFirstPointAboveSecond(currentIndex, nextIndex)));
		}

		std::sort(sortedSidedPoints.begin(), sortedSidedPoints.end(), [&](const SidedPoint &left, const SidedPoint &right)
				{return isFirstPointAboveSecond(left.pointIndex, right.pointIndex);});

		return sortedSidedPoints;
	}

	bool TriangulationAlgorithm::isFirstPointAboveSecond(unsigned int firstIndex, unsigned int secondIndex) const
	{
		if(polygonPoints[firstIndex].Y == polygonPoints[secondIndex].Y)
		{
			return polygonPoints[firstIndex].X < polygonPoints[secondIndex].X;
		}
		return polygonPoints[firstIndex].Y > polygonPoints[secondIndex].Y;
	}

	void TriangulationAlgorithm::determineNeighbor(std::vector<IndexedTriangleMesh> &triangles) const
	{
		//others triangles neighbors
		if(triangles.size() > 1)
		{
			//TODO don't work with cavityTriangulation test
			std::cout<<std::endl;
			std::cout<<triangles[triangles.size()-2].getIndex(0)<<" "<<triangles[triangles.size()-2].getIndex(1)<<" "<<triangles[triangles.size()-2].getIndex(2)<<std::endl;
			std::cout<<triangles[triangles.size()-1].getIndex(0)<<" "<<triangles[triangles.size()-1].getIndex(1)<<" "<<triangles[triangles.size()-1].getIndex(2)<<std::endl;
			std::cout<<std::endl;

			triangles[triangles.size()-1].addNeighbor(1, triangles.size()-2);
			triangles[triangles.size()-2].addNeighbor(0, triangles.size()-1);
		}

		//TODO handle monotone polygon neighbor
	}

	void TriangulationAlgorithm::logOutputData(const std::string &message, const std::vector<IndexedTriangleMesh> &triangles, Logger::CriticalityLevel logLevel) const
	{
		std::stringstream logStream;
		logStream.precision(std::numeric_limits<float>::max_digits10);

		logStream<<message<<std::endl;
		logStream<<"Monotone polygon triangles output data:"<<std::endl;
		for(const auto &triangle : triangles)
		{
			logStream<<" - {"<<polygonPoints[triangle.getIndex(0)]<<"}, {"<<polygonPoints[triangle.getIndex(1)]<<"}, {"<<polygonPoints[triangle.getIndex(2)]<<"}"
                     <<" {"<<polygonPoints[triangle.getNeighbor(0)]<<"}, {"<<polygonPoints[triangle.getNeighbor(1)]<<"}, {"<<polygonPoints[triangle.getNeighbor(2)]<<"}"<<std::endl;
		}
		Logger::logger().log(logLevel, logStream.str());
	}
}
