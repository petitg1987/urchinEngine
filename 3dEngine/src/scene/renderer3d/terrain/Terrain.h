#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/TerrainMaterial.h"
#include "resources/image/Image.h"

namespace urchin
{

    class Terrain
    { //TODO handle: map editor, AI
        public:
            Terrain(const std::string &, float, float, std::unique_ptr<TerrainMaterial> &);
            ~Terrain();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            const std::vector<Point3<float>> &getVertices() const;
            unsigned int getXLength() const;
            unsigned int getZLength() const;
            float getAmbient() const;
            void setAmbient(float);

            void setTransform(const Transform<float> &);
            const Transform<float> &getTransform() const;

            void display(const Matrix4<float> &) const;

        private:
            std::vector<Point3<float>> buildVertices(const Image *) const; //TODO extract these method
            std::vector<Point2<float>> buildTexCoordinates() const;
            std::vector<Vector3<float>> buildNormals() const;
            std::vector<unsigned int> buildIndices() const;

            std::vector<unsigned int> findTriangleIndices(unsigned int) const;

            unsigned int bufferIDs[5], vertexArrayObject;
            enum //buffer IDs indices
            {
                VAO_VERTEX_POSITION = 0,
                VAO_TEX_COORD,
                VAO_NORMAL,
                VAO_INDEX
            };
            enum //shader input
            {
                SHADER_VERTEX_POSITION = 0,
                SHADER_TEX_COORD,
                SHADER_NORMAL
            };
            unsigned int shader;
            int mModelLoc, mProjectionLoc, mViewLoc;
            int ambientLoc;

            float xzScale;
            float yScale;
            Matrix4<float> projectionMatrix;
            float ambient;

            std::unique_ptr<TerrainMaterial> terrainMaterial;

            Transform<float> transform;

            unsigned int xLength, zLength;
            std::vector<Point3<float>> vertices;
            std::vector<Point2<float>> texCoordinates;
            std::vector<Vector3<float>> normals;
            std::vector<unsigned int> indices;
    };

}

#endif
