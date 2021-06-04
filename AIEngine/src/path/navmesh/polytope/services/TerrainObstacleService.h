#pragma once

#include <UrchinCommon.h>

#include <path/navmesh/csg/CSGPolygon.h>

namespace urchin {

    class TerrainObstacleService {
        public:
            enum EdgeDirection {
                LEFT,
                RIGHT,
                TOP,
                BOTTOM
            };

            TerrainObstacleService(std::string name, const Point3<float>&, std::vector<Point3<float>>, unsigned int, unsigned int);

            std::vector<CSGPolygon<float>> computeSelfObstacles(float);

        private:
            bool isWalkableSquare(unsigned int, float) const;
            float computeTriangleSlope(const Point3<float>&, const Point3<float>&, const Point3<float>&) const;

            std::vector<unsigned int> findAllInaccessibleNeighbors(unsigned int, float) const;
            std::vector<unsigned int> retrieveNeighbors(unsigned int) const;

            CSGPolygon<float> squaresToPolygon(const std::vector<unsigned int>&, unsigned int) const;
            unsigned int retrieveNextPointIndex(unsigned int, const std::vector<EdgeDirection>&, const std::vector<unsigned int>&, EdgeDirection&) const;
            int nextPointInDirection(unsigned int, EdgeDirection) const;
            bool edgeBelongToOneSquare(unsigned int, unsigned int, const std::vector<unsigned int>&) const;

            CSGPolygon<float> pointIndicesToPolygon(const std::vector<unsigned int>&, unsigned int) const;

            std::string terrainName;
            Point3<float> position;
            std::vector<Point3<float>> localVertices;
            unsigned int xLength;
            unsigned int zLength;
    };

}
