#pragma once

#include <memory>
#include <UrchinCommon.h>

#include "graphics/api/GraphicsApi.h"
#include "scene/renderer3d/landscape/terrain/TerrainMaterial.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/landscape/terrain/object/TerrainObjectSpawner.h"

namespace urchin {

    class Terrain {
        public:
            Terrain(std::unique_ptr<TerrainMesh>, std::unique_ptr<TerrainMaterials>, const Point3<float>&);

            void initialize(RenderTarget&);

            void setMesh(std::unique_ptr<TerrainMesh>);
            const TerrainMesh* getMesh() const;
            void setMaterials(std::unique_ptr<TerrainMaterials>);
            const TerrainMaterials* getMaterials() const;
            void addObjectSpawner(std::unique_ptr<TerrainObjectSpawner>);

            void setObjectsViewDistancePercentage(float);
            float getObjectsViewDistancePercentage() const;

            void setPosition(const Point3<float>&);
            const Point3<float>& getPosition() const;

            float getAmbient() const;
            void setAmbient(float);

            void setLightMask(uint8_t);
            uint8_t getLightMask() const;

            Point3<float> findNearestPoint(const Point2<float>&) const;
            float findHeight(const Point2<float>&) const;

            void prepareRendering(unsigned int&, const Camera&, float) const;

        private:
            struct TerrainShaderConst {
                float ambient;
                unsigned int lightMask;
            };

            void createOrUpdateRenderer();
            void refreshMaterials() const;

            static constexpr uint32_t PROJ_VIEW_MATRIX_UNIFORM_BINDING = 0;
            static constexpr uint32_t POSITION_UNIFORM_BINDING = 1;
            static constexpr uint32_t ST_UNIFORM_BINDING = 2;
            static constexpr uint32_t MASK_TEX_UNIFORM_BINDING = 3;
            static constexpr std::array<uint32_t, TerrainMaterials::MAX_MATERIAL> MATERIAL_TEX_UNIFORM_BINDING = {4, 5, 6};

            bool isInitialized;
            RenderTarget* renderTarget;

            std::unique_ptr<GenericRenderer> terrainRenderer;
            std::unique_ptr<Shader> terrainShader;

            std::unique_ptr<TerrainMesh> mesh;
            std::unique_ptr<TerrainMaterials> materials;
            std::vector<std::unique_ptr<TerrainObjectSpawner>> objectsSpawner;

            float objectsViewDistancePercentage;
            Point3<float> position;
            float ambient;
            uint8_t lightMask;
    };

}
