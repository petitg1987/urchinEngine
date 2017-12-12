#include <cassert>
#include <cmath>

#include "TerrainObstacleService.h"

namespace urchin
{
    std::vector<CSGPolygon<float>> TerrainObstacleService::selfObstacles(const Point3<float> &position, const std::vector<Point3<float>> &localVertices,
                                                                         unsigned int xLength, unsigned int zLength, float maxSlopeInRadian)
    {
        //TODO implement it
    }

    std::vector<unsigned int> TerrainObstacleService::retrieveNeighbors(unsigned int farLeftPointIndex, unsigned int xLength, unsigned int zLength) const
    {
        #ifdef _DEBUG
            assert((farLeftPointIndex + 1) % xLength != 0); //not an extreme right point
            assert(farLeftPointIndex < (xLength * (zLength - 1))); //not an extreme near point
        #endif

        std::vector<unsigned int> neighbors;
        neighbors.reserve(4);

        //left neighbor
        if(farLeftPointIndex % xLength != 0)
        {
            neighbors.push_back(farLeftPointIndex - 1);
        }

        //right neighbor
        if((farLeftPointIndex + 2) % xLength != 0)
        {
            neighbors.push_back(farLeftPointIndex + 1);
        }

        //far neighbor
        if(farLeftPointIndex >= xLength)
        {
            neighbors.push_back(farLeftPointIndex - xLength);
        }

        //near neighbor
        unsigned int totalPoints = xLength - zLength;
        if(farLeftPointIndex < totalPoints - (2 * xLength))
        {
            neighbors.push_back(farLeftPointIndex + xLength);
        }

        return neighbors;
    }

    float TerrainObstacleService::computeSquareSlope(unsigned int farLeftPointIndex, const std::vector<Point3<float>> &localVertices,
                                                     unsigned int xLength, unsigned int zLength) const
    {
        #ifdef _DEBUG
            assert((farLeftPointIndex + 1) % xLength != 0); //not an extreme right point
            assert(farLeftPointIndex < (xLength * (zLength - 1))); //not an extreme near point
        #endif

        Point3<float> farLeftPoint = localVertices[farLeftPointIndex];
        Point3<float> farRightPoint = localVertices[farLeftPointIndex + 1];
        Point3<float> nearLeftPoint = localVertices[farLeftPointIndex + xLength];
        Point3<float> nearRightPoint = localVertices[farLeftPointIndex + xLength + 1];

        float triangle1Slope = computeTriangleSlope({farLeftPoint, nearLeftPoint, nearRightPoint});
        float triangle2Slope = computeTriangleSlope({farLeftPoint, nearRightPoint, farRightPoint});

        return std::max(triangle1Slope, triangle2Slope);
    }

    float TerrainObstacleService::computeTriangleSlope(const std::vector<Point3<float>> &ccwPoints) const
    {
        Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
        Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
        Vector3<float> normal = v1.crossProduct(v2).normalize();

        Vector3<float> upVector(0.0, 1.0, 0.0);
        return std::acos(normal.dotProduct(upVector));
    }
}
