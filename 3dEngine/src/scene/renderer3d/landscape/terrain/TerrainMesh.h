#ifndef URCHINENGINE_TERRAINMESH_H
#define URCHINENGINE_TERRAINMESH_H

#include <vector>
#include <UrchinCommon.h>

#include <resources/image/Image.h>

namespace urchin {

    class TerrainMesh {
        public:
            TerrainMesh(const std::string&, float, float);

            const std::string& getHeightFilename() const;
            float getXZScale() const;
            float getYScale() const;
            unsigned int getXSize() const;
            unsigned int getZSize() const;

            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<Vector3<float>>& getNormals() const;
            const std::vector<unsigned int>& getIndices() const;

            Point3<float> findPointAt(const Point2<float>&) const;
            float findHeightAt(const Point2<float>&) const;

        private:
            static std::string generateTerrainMeshHash(const std::string&, float, float);

            unsigned int computeNumberVertices() const;
            unsigned int computeNumberIndices() const;
            unsigned int computeNumberNormals() const;

            std::vector<Point3<float>> buildVertices(const Image*);
            std::vector<unsigned int> buildIndices();
            std::vector<Vector3<float>> buildNormals();
            std::vector<unsigned int> findTriangleIndices(unsigned int) const;

            void writeTerrainMeshFile(const std::string&, const std::string&) const;
            static void writeVersion(std::ofstream&, unsigned int);
            static void writeHash(std::ofstream&, const std::string&);

            void loadTerrainMeshFile(std::ifstream&);
            static unsigned int readVersion(std::ifstream&);
            static std::string readHash(std::ifstream&);

            std::string heightFilename;
            float xzScale;
            float yScale;

            unsigned int xSize;
            unsigned int zSize;

            std::vector<Point3<float>> vertices;
            std::vector<Vector3<float>> normals;
            std::vector<unsigned int> indices;
            std::unique_ptr<HeightfieldPointHelper<float>> heightfieldPointHelper;
    };

}

#endif
