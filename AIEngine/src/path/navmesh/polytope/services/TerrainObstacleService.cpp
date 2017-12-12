#include <cassert>
#include <cmath>
#include <stack>

#include "TerrainObstacleService.h"

namespace urchin
{
    std::vector<CSGPolygon<float>> TerrainObstacleService::selfObstacles(const Point3<float> &position, const std::vector<Point3<float>> &localVertices,
                                                                         unsigned int xLength, unsigned int zLength, float maxSlopeInRadian)
    {
        std::vector<CSGPolygon<float>> obstaclePolygons;

        unsigned int maxSquareIndex = (xLength * (zLength - 1));
        std::vector<bool> squaresProcessed(maxSquareIndex, false);
        for(unsigned int squareIndex = 0; squareIndex < maxSquareIndex; ++squareIndex)
        {
            if((squareIndex + 1) % xLength == 0)
            { //extreme right point: not a valid square
                continue;
            }
            if(squaresProcessed[squareIndex])
            {
                continue;
            }

            if(!isWalkableSquare(squareIndex, localVertices, xLength, zLength, maxSlopeInRadian))
            {
                std::vector<unsigned int> inaccessibleSquares = findAllInaccessibleNeighbors(squareIndex, localVertices, xLength, zLength, maxSlopeInRadian);
                obstaclePolygons.emplace_back(squaresToPolygon(inaccessibleSquares, localVertices, xLength));

                squaresProcessed.insert(squaresProcessed.end(), inaccessibleSquares.begin(), inaccessibleSquares.end());
            }
        }

        return obstaclePolygons;
    }

    bool TerrainObstacleService::isWalkableSquare(unsigned int squareIndex, const std::vector<Point3<float>> &localVertices,
                                                  unsigned int xLength, unsigned int zLength, float maxSlopeInRadian) const
    {
        #ifdef _DEBUG
            assert((squareIndex + 1) % xLength != 0); //not an extreme right point
            assert(squareIndex < (xLength * (zLength - 1))); //not an extreme near point
        #endif

        Point3<float> farLeftPoint = localVertices[squareIndex];
        Point3<float> farRightPoint = localVertices[squareIndex + 1];
        Point3<float> nearLeftPoint = localVertices[squareIndex + xLength];
        Point3<float> nearRightPoint = localVertices[squareIndex + xLength + 1];

        float triangle1Slope = computeTriangleSlope({farLeftPoint, nearLeftPoint, nearRightPoint});
        float triangle2Slope = computeTriangleSlope({farLeftPoint, nearRightPoint, farRightPoint});

        return std::max(triangle1Slope, triangle2Slope) <= maxSlopeInRadian;
    }

    float TerrainObstacleService::computeTriangleSlope(const std::vector<Point3<float>> &ccwPoints) const
    {
        Vector3<float> v1 = ccwPoints[0].vector(ccwPoints[2]);
        Vector3<float> v2 = ccwPoints[1].vector(ccwPoints[0]);
        Vector3<float> normal = v1.crossProduct(v2).normalize();

        Vector3<float> upVector(0.0, 1.0, 0.0);
        return std::acos(normal.dotProduct(upVector));
    }

    std::vector<unsigned int> TerrainObstacleService::findAllInaccessibleNeighbors(unsigned int squareIndex, const std::vector<Point3<float>> &localVertices,
                                                                                   unsigned int xLength, unsigned int zLength, float maxSlopeInRadian) const
    {
        std::vector<unsigned int> inaccessibleNeighbors;
        std::stack<unsigned int> squaresToProcess;
        squaresToProcess.push(squareIndex);

        while(!squaresToProcess.empty())
        {
            unsigned int currSquareIndex = squaresToProcess.top();
            squaresToProcess.pop();

            inaccessibleNeighbors.push_back(currSquareIndex);

            std::vector<unsigned int> neighborSquares = retrieveNeighbors(currSquareIndex, xLength, zLength);
            for(unsigned int neighborSquare : neighborSquares)
            {
                if(!isWalkableSquare(neighborSquare, localVertices, xLength, zLength, maxSlopeInRadian))
                { //inaccessible square
                    squaresToProcess.push(neighborSquare);
                }
            }
        }

        return inaccessibleNeighbors;
    }

    std::vector<unsigned int> TerrainObstacleService::retrieveNeighbors(unsigned int squareIndex, unsigned int xLength, unsigned int zLength) const
    {
        #ifdef _DEBUG
            assert((squareIndex + 1) % xLength != 0); //not an extreme right point
            assert(squareIndex < (xLength * (zLength - 1))); //not an extreme near point
        #endif

        std::vector<unsigned int> neighbors;
        neighbors.reserve(4);

        if(squareIndex % xLength != 0)
        { //left neighbor
            neighbors.push_back(squareIndex - 1);
        }

        if((squareIndex + 2) % xLength != 0)
        { //right neighbor
            neighbors.push_back(squareIndex + 1);
        }

        if(squareIndex >= xLength)
        { //far neighbor
            neighbors.push_back(squareIndex - xLength);
        }

        if(squareIndex < xLength * (zLength - 2))
        { //near neighbor
            neighbors.push_back(squareIndex + xLength);
        }

        return neighbors;
    }

    CSGPolygon<float> TerrainObstacleService::squaresToPolygon(const std::vector<unsigned int> &squares, const std::vector<Point3<float>> &localVertices,
                                                               unsigned int xLength) const
    {
        //TODO implement it
    }
}
