#pragma once

#include <string>
#include <vector>
#include <memory>
#include <UrchinCommon.h>

#include <graphic/render/shader/model/Shader.h>
#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/landscape/terrain/TerrainMaterial.h>
#include <scene/renderer3d/landscape/terrain/TerrainMesh.h>
#include <scene/renderer3d/landscape/terrain/grass/TerrainGrass.h>
#include <scene/renderer3d/camera/Camera.h>
#include <resources/image/Image.h>

namespace urchin {

    class Terrain {
        public:
            Terrain(std::unique_ptr<TerrainMesh>, std::unique_ptr<TerrainMaterials>, const Point3<float>&);

            void initialize(RenderTarget&);
            void onCameraProjectionUpdate(const Matrix4<float>&);

            void setMesh(std::unique_ptr<TerrainMesh>);
            const TerrainMesh* getMesh() const;
            void setMaterials(std::unique_ptr<TerrainMaterials>);
            const TerrainMaterials* getMaterials() const;
            TerrainGrass* getGrass() const;

            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const;

            float getAmbient() const;
            void setAmbient(float);

            Point3<float> findPointAt(const Point2<float>&) const;
            float findHeightAt(const Point2<float>&) const;

            void prepareRendering(const Camera&, float invFrameRate) const;

        private:
            void refreshMaterials();
            void refreshGrassMesh();
            void refreshGrassAmbient();

            bool isInitialized;
            RenderTarget* renderTarget;

            std::unique_ptr<GenericRenderer> terrainRenderer;
            std::unique_ptr<Shader> terrainShader;
            struct {
                alignas(16) Matrix4<float> projectionMatrix;
                alignas(16) Point3<float> position;
            } positioningData;

            std::unique_ptr<TerrainMesh> mesh;
            std::unique_ptr<TerrainMaterials> materials;
            std::unique_ptr<TerrainGrass> grass;

            float ambient;
    };

}
