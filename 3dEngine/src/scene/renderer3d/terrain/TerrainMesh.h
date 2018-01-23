#ifndef URCHINENGINE_TERRAINMESH_H
#define URCHINENGINE_TERRAINMESH_H

#include <vector>
#include "UrchinCommon.h"

#include "resources/image/Image.h"

#define RESTART_INDEX 4294967295 //(2^32)-1

namespace urchin
{

    class TerrainMesh
    {
        public:
            TerrainMesh(const std::string &, float, float);

            const std::string &getHeightFilename() const;
            float getXZScale() const;
            float getYScale() const;
            unsigned int getXSize() const;
            unsigned int getZSize() const;

            const std::vector<Point3<float>> &getVertices() const;
            const std::vector<Vector3<float>> &getNormals() const;
            const std::vector<unsigned int> &getIndices() const;

        private:
            std::vector<Point3<float>> buildVertices(const Image *);
            std::vector<unsigned int> buildIndices();
            std::vector<Vector3<float>> buildNormals();
            std::vector<unsigned int> findTriangleIndices(unsigned int) const;

            std::string heightFilename;
            float xzScale;
            float yScale;

            unsigned int xSize;
            unsigned int zSize;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<unsigned int> indices;
    };

}

#endif
