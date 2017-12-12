#ifndef URCHINENGINE_TERRAINOBSTACLESERVICE_H
#define URCHINENGINE_TERRAINOBSTACLESERVICE_H

#include "UrchinCommon.h"

#include "path/navmesh/model/NavMeshConfig.h"
#include "path/navmesh/csg/CSGPolygon.h"

namespace urchin
{

    class TerrainObstacleService : public Singleton<TerrainObstacleService>
    {
        public:
            friend class Singleton<TerrainObstacleService>;

            std::vector<CSGPolygon<float>> selfObstacles(const Point3<float> &, const std::vector<Point3<float>> &, unsigned int, unsigned int, float);

        private:
            TerrainObstacleService() = default;
            ~TerrainObstacleService() override = default;

            std::vector<unsigned int> retrieveNeighbors(unsigned int, unsigned int, unsigned int) const;

            float computeSquareSlope(unsigned int, const std::vector<Point3<float>> &, unsigned int, unsigned int) const;
            float computeTriangleSlope(const std::vector<Point3<float>> &) const;
    };

}

#endif
