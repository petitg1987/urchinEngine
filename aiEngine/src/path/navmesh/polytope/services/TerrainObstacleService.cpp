#include <cassert>
#include <cmath>
#include <stack>
#include <algorithm>

#include <path/navmesh/polytope/services/TerrainObstacleService.h>

namespace urchin {
    TerrainObstacleService::TerrainObstacleService(std::string terrainName, const Point3<float>& position, std::vector<Point3<float>> localVertices,
                                                   unsigned int xLength, unsigned int zLength) :
            terrainName(std::move(terrainName)),
            position(position),
            localVertices(std::move(localVertices)),
            xLength(xLength),
            zLength(zLength) {

    }

    std::vector<CSGPolygon<float>> TerrainObstacleService::computeSelfObstacles(float maxSlopeInRadian) const {
        std::vector<CSGPolygon<float>> obstaclePolygons;

        float maxSlopeDotProduct = std::cos(maxSlopeInRadian);

        unsigned int obstacleIndex = 0;
        unsigned int maxSquareIndex = (xLength * (zLength - 1));
        std::vector squaresProcessed(maxSquareIndex, false);
        for (unsigned int squareIndex = 0; squareIndex < maxSquareIndex; ++squareIndex) {
            if ((squareIndex + 1) % xLength == 0) { //extreme right point: not a valid square
                continue;
            }
            if (squaresProcessed[squareIndex]) {
                continue;
            }

            if (!isWalkableSquare(squareIndex, maxSlopeDotProduct)) {
                std::vector<unsigned int> inaccessibleSquares = findAllInaccessibleNeighbors(squareIndex, maxSlopeDotProduct);
                obstaclePolygons.emplace_back(squaresToPolygon(inaccessibleSquares, obstacleIndex++));

                for (unsigned int inaccessibleSquare : inaccessibleSquares) {
                    squaresProcessed[inaccessibleSquare] = true;
                }
            }
        }

        return obstaclePolygons;
    }

    bool TerrainObstacleService::isWalkableSquare(unsigned int squareIndex, float maxSlopeDotProduct) const {
        assert((squareIndex + 1) % xLength != 0); //not an extreme right point
        assert(squareIndex < (xLength * (zLength - 1))); //not an extreme near point

        Point3<float> farLeftPoint = localVertices[squareIndex];
        Point3<float> farRightPoint = localVertices[squareIndex + 1];
        Point3<float> nearLeftPoint = localVertices[squareIndex + xLength];
        Point3<float> nearRightPoint = localVertices[squareIndex + xLength + 1];

        float triangle1Slope = computeTriangleSlope(farLeftPoint, nearLeftPoint, nearRightPoint);
        float triangle2Slope = computeTriangleSlope(farLeftPoint, nearRightPoint, farRightPoint);

        return std::min(triangle1Slope, triangle2Slope) >= maxSlopeDotProduct;
    }

    float TerrainObstacleService::computeTriangleSlope(const Point3<float>& p0, const Point3<float>& p1, const Point3<float>& p2) const {
        Vector3<float> v1 = p0.vector(p2);
        Vector3<float> v2 = p1.vector(p0);
        Vector3<float> normal = v1.crossProduct(v2).normalize();

        Vector3 upVector(0.0f, 1.0f, 0.0f);
        return normal.dotProduct(upVector);
    }

    std::vector<unsigned int> TerrainObstacleService::findAllInaccessibleNeighbors(unsigned int squareIndex, float maxSlopeDotProduct) const {
        std::vector<unsigned int> inaccessibleNeighbors;
        std::stack<unsigned int> squaresToProcess;
        squaresToProcess.push(squareIndex);

        while (!squaresToProcess.empty()) {
            unsigned int currSquareIndex = squaresToProcess.top();
            squaresToProcess.pop();

            inaccessibleNeighbors.push_back(currSquareIndex);

            std::vector<unsigned int> neighborSquares = retrieveNeighbors(currSquareIndex);
            for (unsigned int neighborSquare : neighborSquares) {
                bool notProcessed = std::ranges::find(inaccessibleNeighbors, neighborSquare) == inaccessibleNeighbors.end();
                if (notProcessed && !isWalkableSquare(neighborSquare, maxSlopeDotProduct)) { //inaccessible square
                    squaresToProcess.push(neighborSquare);
                }
            }
        }

        return inaccessibleNeighbors;
    }

    std::vector<unsigned int> TerrainObstacleService::retrieveNeighbors(unsigned int squareIndex) const {
        assert((squareIndex + 1) % xLength != 0); //not an extreme right point
        assert(squareIndex < (xLength * (zLength - 1))); //not an extreme near point

        std::vector<unsigned int> neighbors;
        neighbors.reserve(4);

        if (squareIndex % xLength != 0) { //left neighbor
            neighbors.push_back(squareIndex - 1);
        }

        if ((squareIndex + 2) % xLength != 0) { //right neighbor
            neighbors.push_back(squareIndex + 1);
        }

        if (squareIndex >= xLength) { //far neighbor
            neighbors.push_back(squareIndex - xLength);
        }

        if (squareIndex < xLength * (zLength - 2)) { //near neighbor
            neighbors.push_back(squareIndex + xLength);
        }

        return neighbors;
    }

    CSGPolygon<float> TerrainObstacleService::squaresToPolygon(const std::vector<unsigned int>& squares, unsigned int obstacleIndex) const {
        std::map<EdgeDirection, std::vector<EdgeDirection>> checkDirectionsMap;
        checkDirectionsMap[RIGHT] = {TOP, RIGHT, BOTTOM};
        checkDirectionsMap[BOTTOM] = {RIGHT, BOTTOM, LEFT};
        checkDirectionsMap[LEFT] = {BOTTOM, LEFT, TOP};
        checkDirectionsMap[TOP] = {LEFT, TOP, RIGHT};

        std::vector<unsigned int> cwPolygonPointIndices;
        cwPolygonPointIndices.push_back(squares[0]);
        cwPolygonPointIndices.push_back(squares[0] + 1);
        EdgeDirection direction = RIGHT;

        while (true) {
            unsigned int lastPointIndex = cwPolygonPointIndices[cwPolygonPointIndices.size() - 1];
            std::vector<EdgeDirection> checkDirections = checkDirectionsMap.at(direction);
            EdgeDirection usedDirection;
            unsigned int nextPointIndex = retrieveNextPointIndex(lastPointIndex, checkDirections, squares, usedDirection);

            if (nextPointIndex == cwPolygonPointIndices[0]) { //polygon end reached
                if (direction == usedDirection) {
                    cwPolygonPointIndices.pop_back();
                }
                break;
            }

            if (direction == usedDirection) {
                cwPolygonPointIndices[cwPolygonPointIndices.size() - 1] = nextPointIndex;
            } else {
                cwPolygonPointIndices.push_back(nextPointIndex);
                direction = usedDirection;
            }
        }

        return pointIndicesToPolygon(cwPolygonPointIndices, obstacleIndex);
    }

    unsigned int TerrainObstacleService::retrieveNextPointIndex(unsigned int pointIndex, const std::vector<EdgeDirection>& checkDirections,
                                                                const std::vector<unsigned int>& squares, EdgeDirection& usedDirection) const {
        for (EdgeDirection checkDirection : checkDirections) {
            int nextPointIndex = nextPointInDirection(pointIndex, checkDirection);
            if (nextPointIndex != -1 && edgeBelongToOneSquare(pointIndex, (unsigned int)nextPointIndex, squares)) {
                usedDirection = checkDirection;
                return (unsigned int)nextPointIndex;
            }
        }

        throw std::runtime_error("No next point found for index: " + std::to_string(pointIndex));
    }

    int TerrainObstacleService::nextPointInDirection(unsigned int pointIndex, EdgeDirection direction) const {
        if (RIGHT == direction) {
            if ((pointIndex + 1) % xLength == 0) { //no right point
                return -1;
            }
            return (int)pointIndex + 1;
        } else if (BOTTOM == direction) {
            if (pointIndex >= xLength * (zLength - 1)) { //no bottom point
                return -1;
            }
            return (int)(pointIndex + xLength);
        } else if (LEFT == direction) {
            if (pointIndex % xLength == 0) { //no left point
                return -1;
            }
            return (int)pointIndex - 1;
        } else if (TOP == direction) {
            if (pointIndex < xLength) { //no top point
                return -1;
            }
            return (int)(pointIndex - xLength);
        }

        throw std::runtime_error("Unknown edge direction: " + std::to_string(direction));
    }

    bool TerrainObstacleService::edgeBelongToOneSquare(unsigned int point1, unsigned int point2, const std::vector<unsigned int>& squares) const {
        return std::ranges::any_of(squares, [&](const auto& squareIndex) {
            return ((squareIndex == point1) || (squareIndex + 1 == point1) || (squareIndex + xLength == point1) || (squareIndex + xLength + 1 == point1)) &&
                   ((squareIndex == point2) || (squareIndex + 1 == point2) || (squareIndex + xLength == point2) || (squareIndex + xLength + 1 == point2));
        });
    }

    CSGPolygon<float> TerrainObstacleService::pointIndicesToPolygon(const std::vector<unsigned int>& cwPolygonPointIndices, unsigned int obstacleIndex) const {
        std::vector<Point2<float>> cwPoints;
        cwPoints.reserve(cwPolygonPointIndices.size());

        for (unsigned int cwPolygonPointIndex : cwPolygonPointIndices) {
            Point3<float> vertex = localVertices[cwPolygonPointIndex] + position;
            cwPoints.emplace_back(Point2(vertex.X, -vertex.Z));
        }

        std::string obstacleName = terrainName + "_obstacle" + std::to_string(obstacleIndex);
        return CSGPolygon(obstacleName, std::move(cwPoints));
    }

}
