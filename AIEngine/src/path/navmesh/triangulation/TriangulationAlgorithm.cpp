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

    TriangleEdge::TriangleEdge(unsigned int triangleIndex, unsigned int edgeIndex) :
            triangleIndex(triangleIndex), edgeIndex(edgeIndex)
    {

    }

	/**
	 * @param ccwPolygonPoints Polygon points in counter clockwise order. Points must be unique.
	 */
	TriangulationAlgorithm::TriangulationAlgorithm(const std::vector<Point2<float>> &ccwPolygonPoints, const std::string &name, TriangleOrientation triangleOrientation) :
			polygonPoints(ccwPolygonPoints)
	{
        #ifdef _DEBUG
			//assert counter-clockwise order
            double area = 0.0;
            for (unsigned int i = 0, prevI = ccwPolygonPoints.size() - 1; i < ccwPolygonPoints.size(); prevI=i++)
            {
                area += (ccwPolygonPoints[i].X - ccwPolygonPoints[prevI].X) * (ccwPolygonPoints[i].Y + ccwPolygonPoints[prevI].Y);
            }
            assert(area <= 0.0);
        #endif

		this->endContourIndices.push_back(ccwPolygonPoints.size());
		this->contourNames.push_back(name);
		this->triangleOrientation = triangleOrientation;
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
	unsigned int TriangulationAlgorithm::addHolePoints(const std::vector<Point2<float>> &cwHolePoints, const std::string &holeName)
	{
        #ifdef _DEBUG
			//assert clockwise order
            double area = 0.0;
            for (unsigned int i = 0, prevI = cwHolePoints.size() - 1; i < cwHolePoints.size(); prevI=i++)
            {
                area += (cwHolePoints[i].X - cwHolePoints[prevI].X) * (cwHolePoints[i].Y + cwHolePoints[prevI].Y);
            }
            assert(area >= 0.0);
        #endif

		polygonPoints.insert(polygonPoints.end(), cwHolePoints.begin(), cwHolePoints.end());
		endContourIndices.push_back(polygonPoints.size());
		contourNames.push_back(holeName);

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

	const std::vector<NavTriangle> &TriangulationAlgorithm::triangulate()
	{ //based on "Computational Geometry - Algorithms and Applications, 3rd Ed" - "Polygon Triangulation"
		#ifdef _DEBUG
            //assert no duplicate points
			for (unsigned int i = 0; i < polygonPoints.size(); ++i)
			{
				for (unsigned int j = 0; j < polygonPoints.size(); ++j)
				{
					assert(i == j || polygonPoints[i].X != polygonPoints[j].X || polygonPoints[i].Y != polygonPoints[j].Y);
				}
			}
		#endif

		std::vector<MonotonePolygon> monotonePolygons = MonotonePolygonAlgorithm(polygonPoints, endContourIndices, contourNames).createYMonotonePolygons();

		triangles.clear();
		triangles.reserve((polygonPoints.size()-2) + (2*getHolesSize()));

		for (const auto &monotonePolygon : monotonePolygons)
		{
            std::vector<NavTriangle> monotonePolygonTriangles = triangulateMonotonePolygon(monotonePolygon);
            triangles.insert(triangles.end(), monotonePolygonTriangles.begin(), monotonePolygonTriangles.end());
		}

		if(!sharedMonotoneEdges.empty())
		{
			logOutputData("Missing neighbors (" + std::to_string(sharedMonotoneEdges.size()) + ") between monotone polygons", triangles, Logger::ERROR);
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

    std::vector<NavTriangle> TriangulationAlgorithm::triangulateMonotonePolygon(const MonotonePolygon &monotonePolygon)
	{
        missingTriangleNeighbor = 0;
        const std::vector<unsigned int> &monotonePolygonPoints = monotonePolygon.getCcwPoints();
		std::vector<SidedPoint> sortedSidedPoints = buildSortedSidedPoints(monotonePolygonPoints);

        std::vector<NavTriangle> monotoneTriangles;
        monotoneTriangles.reserve(monotonePolygonPoints.size());

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

                    monotoneTriangles.emplace_back(buildOrientedTriangle(currentPoint.pointIndex, topPoint.pointIndex, top2Point.pointIndex));
                    determineNeighbors(monotoneTriangles, monotonePolygon);
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
                        monotoneTriangles.emplace_back(buildOrientedTriangle(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
                        determineNeighbors(monotoneTriangles, monotonePolygon);
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

            monotoneTriangles.emplace_back(buildOrientedTriangle(currentPoint.pointIndex, top2Point.pointIndex, topPoint.pointIndex));
            determineNeighbors(monotoneTriangles, monotonePolygon);
		}

        if(missingTriangleNeighbor!=0)
        {
            logOutputData("Missing neighbors (" + std::to_string(missingTriangleNeighbor) + ") on monotone polygon", monotoneTriangles, Logger::ERROR);
        }

		#ifdef _DEBUG
//      	logOutputData("Output of triangulation algorithm", monotoneTriangles, Logger::INFO);
    	#endif

        return monotoneTriangles;
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

	NavTriangle TriangulationAlgorithm::buildOrientedTriangle(unsigned int pointIndex1, unsigned int pointIndex2, unsigned int pointIndex3) const
	{
		if(triangleOrientation==TriangulationAlgorithm::NONE)
		{
			return NavTriangle(pointIndex1, pointIndex2, pointIndex3);
		}else if(triangleOrientation==TriangulationAlgorithm::CCW)
		{
			Vector2<double> v1 = polygonPoints[pointIndex1].template cast<double>().vector(polygonPoints[pointIndex2].template cast<double>());
			Vector2<double> v2 = polygonPoints[pointIndex2].template cast<double>().vector(polygonPoints[pointIndex3].template cast<double>());

			double crossProductZ = v1.X*v2.Y - v1.Y*v2.X;
			if(crossProductZ > 0.0)
			{
				return NavTriangle(pointIndex1, pointIndex2, pointIndex3);
			}else
			{
				return NavTriangle(pointIndex2, pointIndex1, pointIndex3);
			}
		}

		throw std::runtime_error("Unknown triangle orientation type: " + triangleOrientation);
	}

    void TriangulationAlgorithm::determineNeighbors(std::vector<NavTriangle> &triangles, const MonotonePolygon &monotonePolygon)
    {
        determineNeighborsInsideMonotone(triangles);
        determineNeighborsBetweenMonotones(triangles, monotonePolygon);
    }

    void TriangulationAlgorithm::determineNeighborsInsideMonotone(std::vector<NavTriangle> &monotoneTriangles)
    {
        int currMonotoneTriangleIndex = monotoneTriangles.size()-1;
        int prevMonotoneTriangleIndex = monotoneTriangles.size()-2;
		NavTriangle &currTriangle = monotoneTriangles[currMonotoneTriangleIndex];

        missingTriangleNeighbor += monotoneTriangles.size()>1 ? 1 : 0; //don't expect neighbor for first triangle
        while(prevMonotoneTriangleIndex>=0 && missingTriangleNeighbor>0)
        {
			NavTriangle &prevTriangle = monotoneTriangles[prevMonotoneTriangleIndex];

            for(unsigned int prevEdgeIndex=2, edgeIndex=0; edgeIndex<3 && missingTriangleNeighbor>0; prevEdgeIndex=edgeIndex++)
            {
                if (areSameEdge(prevTriangle, prevEdgeIndex, edgeIndex, currTriangle, 0, 1))
                {
                    currTriangle.addNeighbor(0, prevMonotoneTriangleIndex + triangles.size());
                    prevTriangle.addNeighbor(prevEdgeIndex, currMonotoneTriangleIndex + triangles.size());
                    missingTriangleNeighbor--;
                } else if (areSameEdge(prevTriangle, prevEdgeIndex, edgeIndex, currTriangle, 1, 2))
                {
                    currTriangle.addNeighbor(1, prevMonotoneTriangleIndex + triangles.size());
                    prevTriangle.addNeighbor(prevEdgeIndex, currMonotoneTriangleIndex + triangles.size());
                    missingTriangleNeighbor--;
                } else if (areSameEdge(prevTriangle, prevEdgeIndex, edgeIndex, currTriangle, 2, 0))
                {
                    currTriangle.addNeighbor(2, prevMonotoneTriangleIndex + triangles.size());
                    prevTriangle.addNeighbor(prevEdgeIndex, currMonotoneTriangleIndex + triangles.size());
                    missingTriangleNeighbor--;
                }
            }

            prevMonotoneTriangleIndex--;
        }
    }

    void TriangulationAlgorithm::determineNeighborsBetweenMonotones(std::vector<NavTriangle> &monotoneTriangles, const MonotonePolygon &monotonePolygon)
    {
        unsigned int currMonotoneTriangleIndex = monotoneTriangles.size()-1;
		NavTriangle &currTriangle = monotoneTriangles[currMonotoneTriangleIndex];

        for(unsigned int prevEdgeIndex=2, edgeIndex=0; edgeIndex<3; prevEdgeIndex=edgeIndex++)
        {
            if(currTriangle.getNeighbor(prevEdgeIndex)==-1)
            {
				unsigned int edgeStartIndex = currTriangle.getIndex(prevEdgeIndex);
				unsigned int edgeEndIndex = currTriangle.getIndex(edgeIndex);

				if(monotonePolygon.isSharedEdge(edgeStartIndex, edgeEndIndex))
				{
					uint_fast64_t edgeId = computeEdgeId(edgeStartIndex, edgeEndIndex);
					auto itFind = sharedMonotoneEdges.find(edgeId);
					if (itFind == sharedMonotoneEdges.end())
					{
						sharedMonotoneEdges.insert(std::make_pair(edgeId, TriangleEdge(triangles.size() + currMonotoneTriangleIndex, prevEdgeIndex)));
					} else
					{
						currTriangle.addNeighbor(prevEdgeIndex, itFind->second.triangleIndex);
						triangles[(itFind->second.triangleIndex)].addNeighbor(itFind->second.edgeIndex, triangles.size() + currMonotoneTriangleIndex);

						sharedMonotoneEdges.erase(itFind);
					}
				}
            }
        }
    }

    bool TriangulationAlgorithm::areSameEdge(const NavTriangle &triangle1, unsigned int tri1Point1, unsigned int tri1Point2,
                                             const NavTriangle &triangle2, unsigned int tri2Point1, unsigned int tri2Point2) const
    {
        return (triangle1.getIndex(tri1Point1)==triangle2.getIndex(tri2Point1) && triangle1.getIndex(tri1Point2)==triangle2.getIndex(tri2Point2))
               || (triangle1.getIndex(tri1Point1)==triangle2.getIndex(tri2Point2) && triangle1.getIndex(tri1Point2)==triangle2.getIndex(tri2Point1));
    }

    uint_fast64_t TriangulationAlgorithm::computeEdgeId(unsigned int edgeStartIndex, unsigned int edgeEndIndex) const
    {
        auto edgeId = static_cast<uint_fast64_t>(std::min(edgeStartIndex, edgeEndIndex));
        edgeId = edgeId << 32;
        return edgeId + std::max(edgeStartIndex, edgeEndIndex);
    }

	void TriangulationAlgorithm::logOutputData(const std::string &message, const std::vector<NavTriangle> &triangles, Logger::CriticalityLevel logLevel) const
	{
		std::stringstream logStream;
		logStream.precision(std::numeric_limits<float>::max_digits10);

		logStream<<message<<std::endl;
		logStream<<"Monotone polygon triangles output data:"<<std::endl;
		for(const auto &triangle : triangles)
		{
			logStream<<" - {"<<triangle.getIndex(0)<<": "<<polygonPoints[triangle.getIndex(0)]
                     <<"}, {"<<triangle.getIndex(1)<<": "<<polygonPoints[triangle.getIndex(1)]
                     <<"}, {"<<triangle.getIndex(2)<<": "<<polygonPoints[triangle.getIndex(2)]<<"}"
                     <<" {"<<triangle.getNeighbor(0)<<", "<<triangle.getNeighbor(1)<<", "<<triangle.getNeighbor(2)<<"}"<<std::endl;
		}
		Logger::logger().log(logLevel, logStream.str());
	}
}
