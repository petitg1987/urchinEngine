#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/TerrainMaterial.h"
#include "scene/renderer3d/terrain/TerrainMesh.h"
#include "resources/image/Image.h"

namespace urchin
{

    class Terrain
    { //TODO handle: map editor, AI
        public:
            Terrain(std::unique_ptr<TerrainMesh> &, std::unique_ptr<TerrainMaterial> &);
            ~Terrain();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            const std::vector<Point3<float>> &getVertices() const;
            unsigned int getXSize() const;
            unsigned int getZSize() const;
            float getAmbient() const;
            void setAmbient(float);

            void setTransform(const Transform<float> &);
            const Transform<float> &getTransform() const;

            void display(const Matrix4<float> &) const;

        private:
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

            Matrix4<float> projectionMatrix;

            std::unique_ptr<TerrainMesh> terrainMesh;
            std::unique_ptr<TerrainMaterial> terrainMaterial;
            float ambient;
            Transform<float> transform;
    };

}

#endif
