#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/TerrainMaterial.h"
#include "scene/renderer3d/terrain/TerrainMesh.h"
#include "scene/renderer3d/terrain/TerrainGrass.h"
#include "scene/renderer3d/camera/Camera.h"
#include "resources/image/Image.h"

namespace urchin
{

    class Terrain
    {
        public:
            Terrain(std::unique_ptr<TerrainMesh> &, std::unique_ptr<TerrainMaterial> &);
            ~Terrain();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void setMesh(std::unique_ptr<TerrainMesh> &);
            const TerrainMesh *getMesh() const;
            void setMaterial(std::unique_ptr<TerrainMaterial> &);
            const TerrainMaterial *getMaterial() const;
            void setGrass(std::unique_ptr<TerrainGrass> &);
            const TerrainGrass *getGrass();

            void setPosition(const Point3<float> &);
            const Point3<float> &getPosition() const;

            float getAmbient() const;
            void setAmbient(float);

            void display(const Camera *, float invFrameRate) const;

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
            unsigned int terrainShader;
            int vPositionLoc, mProjectionLoc, mViewLoc;
            int ambientLoc;

            Matrix4<float> projectionMatrix;

            std::unique_ptr<TerrainMesh> mesh;
            std::unique_ptr<TerrainMaterial> material;
            std::unique_ptr<TerrainGrass> grass;
            Point3<float> position;
            float ambient;
    };

}

#endif
