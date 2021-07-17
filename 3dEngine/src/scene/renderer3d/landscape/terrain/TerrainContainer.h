#pragma once

#include <UrchinCommon.h>

#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/terrain/Terrain.h>

namespace urchin {

    class TerrainContainer {
        public:
            struct Config {
                float grassDisplayDistance = 100;
            };

            explicit TerrainContainer(RenderTarget&);

            void onCameraProjectionUpdate(const Camera&);

            void addTerrain(const std::shared_ptr<Terrain>&);
            void removeTerrain(const Terrain&);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void prepareRendering(const Camera&, float) const;

        private:
            void updateAllTerrainConfig();
            void updateTerrainConfig(Terrain&) const;

            RenderTarget& renderTarget;

            std::vector<std::shared_ptr<Terrain>> terrains;

            Matrix4<float> projectionMatrix;

            Config config;
    };

}
