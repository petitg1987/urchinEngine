#include <cassert>
#include <cmath>
#include <stack>
#include <algorithm>

#include "TerrainObstacleService.h"

namespace urchin
{
    TerrainObstacleService::TerrainObstacleService(std::string terrainName, const Point3<float> &position, const std::vector<Point3<float>> &localVertices,
                                                   unsigned int xLength, unsigned int zLength) :
            terrainName(std::move(terrainName)),
            position(position),
            localVertices(localVertices),
            xLength(xLength),
            zLength(zLength)
    {

    }

    std::vector<CSGPolygon<float>> TerrainObstacleService::computeSelfObstacles(float maxSlopeInRadian)
    {
        std::vector<CSGPolygon<float>> obstaclePolygons;

        unsigned int obstacleIndex = 0;
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

            if(!isWalkableSquare(squareIndex, maxSlopeInRadian))
            {
                std::vector<unsigned int> inaccessibleSquares = findAllInaccessibleNeighbors(squareIndex, maxSlopeInRadian);
                obstaclePolygons.emplace_back(squaresToPolygon(inaccessibleSquares, obstacleIndex++));

                for(unsigned int inaccessibleSquare : inaccessibleSquares)
                {
                    squaresProcessed[inaccessibleSquare] = true;
                }
            }
        }

        return obstaclePolygons;
    }

    bool TerrainObstacleService::isWalkableSquare(unsigned int squareIndex, float maxSlopeInRadian) const
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

    std::vector<unsigned int> TerrainObstacleService::findAllInaccessibleNeighbors(unsigned int squareIndex, float maxSlopeInRadian) const
    {
        std::vector<unsigned int> inaccessibleNeighbors;
        std::stack<unsigned int> squaresToProcess;
        squaresToProcess.push(squareIndex);

        while(!squaresToProcess.empty())
        {
            unsigned int currSquareIndex = squaresToProcess.top();
            squaresToProcess.pop();

            inaccessibleNeighbors.push_back(currSquareIndex);

            std::vector<unsigned int> neighborSquares = retrieveNeighbors(currSquareIndex);
            for(unsigned int neighborSquare : neighborSquares)
            {
                bool notProcessed = std::find(inaccessibleNeighbors.begin(), inaccessibleNeighbors.end(), neighborSquare) == inaccessibleNeighbors.end();
                if(notProcessed && !isWalkableSquare(neighborSquare, maxSlopeInRadian))
                { //inaccessible square
                    squaresToProcess.push(neighborSquare);
                }
            }
        }

        return inaccessibleNeighbors;
    }

    std::vector<unsigned int> TerrainObstacleService::retrieveNeighbors(unsigned int squareIndex) const
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

    CSGPolygon<float> TerrainObstacleService::squaresToPolygon(const std::vector<unsigned int> &squares, unsigned int obstacleIndex) const
    {
        std::map<EdgeDirection, std::vector<EdgeDirection>> checkDirectionsMap;
        checkDirectionsMap[EdgeDirection::RIGHT] = {EdgeDirection::TOP, EdgeDirection::RIGHT, EdgeDirection::BOTTOM};
        checkDirectionsMap[EdgeDirection::BOTTOM] = {EdgeDirection::RIGHT, EdgeDirection::BOTTOM, EdgeDirection::LEFT};
        checkDirectionsMap[EdgeDirection::LEFT] = {EdgeDirection::BOTTOM, EdgeDirection::LEFT, EdgeDirection::TOP};
        checkDirectionsMap[EdgeDirection::TOP] = {EdgeDirection::LEFT, EdgeDirection::TOP, EdgeDirection::RIGHT};

        std::vector<unsigned int> cwPolygonPointIndices;
        cwPolygonPointIndices.push_back(squares[0]);
        cwPolygonPointIndices.push_back(squares[0] + 1);
        EdgeDirection direction = EdgeDirection::RIGHT;

        while(true)
        {
            unsigned int lastPointIndex = cwPolygonPointIndices[cwPolygonPointIndices.size() - 1];
            std::vector<EdgeDirection> checkDirections = checkDirectionsMap.at(direction);
            EdgeDirection usedDirection;
            unsigned int nextPointIndex = retrieveNextPointIndex(lastPointIndex, checkDirections, squares, usedDirection);

            if(nextPointIndex==cwPolygonPointIndices[0])
            { //polygon end reached
                if(direction==usedDirection)
                {
                    cwPolygonPointIndices.pop_back();
                }
                break;
            }

            if(direction==usedDirection)
            {
                cwPolygonPointIndices[cwPolygonPointIndices.size() - 1] = nextPointIndex;
            }else
            {
                cwPolygonPointIndices.push_back(nextPointIndex);
                direction = usedDirection;
            }
        }

        return pointIndicesToPolygon(cwPolygonPointIndices, obstacleIndex);
    }

    unsigned int TerrainObstacleService::retrieveNextPointIndex(unsigned int pointIndex, const std::vector<EdgeDirection> &checkDirections,
                                                                const std::vector<unsigned int> &squares, EdgeDirection &usedDirection) const
    {
        for(EdgeDirection checkDirection : checkDirections)
        {
            int nextPoint = nextPointInDirection(pointIndex, checkDirection);
            if(nextPoint!=-1 && pointExistInSquares(static_cast<unsigned int>(nextPoint), squares))
            {
                usedDirection = checkDirection;
                return static_cast<unsigned int>(nextPoint);
            }
        }

        throw std::runtime_error("No next point found for index: " + std::to_string(pointIndex));
    }

    int TerrainObstacleService::nextPointInDirection(unsigned int pointIndex, EdgeDirection direction) const
    {
        if(EdgeDirection::RIGHT==direction)
        {
            if((pointIndex + 1) % xLength == 0)
            { //no right point
                return -1;
            }
            return pointIndex + 1;
        }else if(EdgeDirection::BOTTOM==direction)
        {
            if(pointIndex >= xLength * (zLength - 1))
            {
                return -1;
            }
            return pointIndex + xLength;
        }else if(EdgeDirection::LEFT==direction)
        {
            if(pointIndex % xLength == 0)
            { //no left point
                return -1;
            }
            return pointIndex - 1;
        }else if(EdgeDirection::TOP==direction)
        {
            if(pointIndex < xLength)
            {
                return -1;
            }
            return pointIndex - xLength;
        }

        throw std::runtime_error("Unknown edge direction: " + std::to_string(direction));
    }

    bool TerrainObstacleService::pointExistInSquares(unsigned int pointIndex, const std::vector<unsigned int> &squares) const
    {
        for(unsigned int squareIndex : squares)
        {
            if(     (squareIndex == pointIndex) ||
                    (squareIndex + 1 == pointIndex) ||
                    (squareIndex + xLength == pointIndex) ||
                    (squareIndex + xLength + 1 == pointIndex) )
            {
                return true;
            }
        }
        return false;
    }

    CSGPolygon<float> TerrainObstacleService::pointIndicesToPolygon(const std::vector<unsigned int> &cwPolygonPointIndices, unsigned int obstacleIndex) const
    {
        std::vector<Point2<float>> cwPoints;
        cwPoints.reserve(cwPolygonPointIndices.size());

        for(unsigned int cwPolygonPointIndex : cwPolygonPointIndices)
        {
            Point3<float> vertex = localVertices[cwPolygonPointIndex] + position;
            cwPoints.emplace_back(Point2<float>(vertex.X, -vertex.Z));
        }

        std::string obstacleName = terrainName + "_obstacle" + std::to_string(obstacleIndex);
        return CSGPolygon<float>(obstacleName, cwPoints);
    }

}
