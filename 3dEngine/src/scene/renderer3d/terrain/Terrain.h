#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "scene/renderer3d/terrain/TerrainMaterial.h"
#include "scene/renderer3d/terrain/TerrainMesh.h"
#include "scene/renderer3d/terrain/grass/TerrainGrass.h"
#include "scene/renderer3d/camera/Camera.h"
#include "resources/image/Image.h"

namespace urchin
{

    class Terrain
    {
        public:
            Terrain(std::shared_ptr<TerrainMesh> &, std::unique_ptr<TerrainMaterial> &, const Point3<float> &);
            ~Terrain();

            void onCameraProjectionUpdate(const Matrix4<float> &);

            void setMesh(const std::shared_ptr<TerrainMesh> &);
            const TerrainMesh *getMesh() const;
            void setMaterial(std::unique_ptr<TerrainMaterial> &);
            const TerrainMaterial *getMaterial() const;
            TerrainGrass *getGrass() const;

            void setPosition(const Point3<float> &);
            const Point3<float> &getPosition() const;

            float getAmbient() const;
            void setAmbient(float);

            Point3<float> findPointAt(const Point2<float> &) const;
            float findHeightAt(const Point2<float> &) const;

            void display(const Camera *, float invFrameRate) const;

        private:
            void refreshMaterial();
            void refreshGrassMesh();
            void refreshGrassAmbient();

            unsigned int bufferIDs[4], vertexArrayObject;
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
            int sRepeatLoc, tRepeatLoc;

            Matrix4<float> projectionMatrix;

            std::shared_ptr<TerrainMesh> mesh;
            std::unique_ptr<TerrainMaterial> material;
            std::unique_ptr<TerrainGrass> grass;
            Point3<float> position;
            float ambient;
    };

}

#endif
