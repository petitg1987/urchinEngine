#pragma once

#include <vector>
#include <UrchinCommon.h>

#include "resources/image/Image.h"
#include "scene/renderer3d/landscape/terrain/TerrainMeshMode.h"

namespace urchin {

    class TerrainMesh {
        public:
            TerrainMesh(std::string, float, float, TerrainMeshMode);

            const std::string& getHeightFilename() const;
            float getXZScale() const;
            float getYScale() const;
            unsigned int getXSize() const;
            unsigned int getZSize() const;
            TerrainMeshMode getMode() const;

            const std::vector<Point3<float>>& getRawVertices() const;
            const std::vector<Point3<float>>& getVertices() const;
            const std::vector<unsigned int>& getIndices() const;
            const std::vector<Vector3<float>>& getNormals() const;

            Point3<float> findPointAt(const Point2<float>&) const;
            float findHeightAt(const Point2<float>&) const;

        private:
            unsigned int computeNumberRawVertices() const;
            unsigned int computeNumberVertices() const;
            unsigned int computeNumberIndices() const;
            unsigned int computeNumberTriangles() const;
            unsigned int computeNumberVertexNormals() const;

            void buildVertices(const Image&);
            void buildIndices();
            void buildNormals();
            std::vector<unsigned int> findTriangleIndices(unsigned int) const;

            std::string heightFilename;
            float xzScale;
            float yScale;
            TerrainMeshMode mode;

            unsigned int xSize;
            unsigned int zSize;

            std::vector<Point3<float>> rawVertices;
            std::vector<Point3<float>> vertices; //either vertices or indices
            std::vector<unsigned int> indices; //either vertices or indices
            std::vector<Vector3<float>> normals;
            std::unique_ptr<HeightfieldPointHelper<float>> heightfieldPointHelper;
    };

}
