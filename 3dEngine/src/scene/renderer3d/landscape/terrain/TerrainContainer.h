#pragma once

#include <UrchinCommon.h>

#include <graphics/api/GraphicsApi.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/terrain/Terrain.h>

namespace urchin {

    class TerrainContainer {
        public:
            struct Config {
                float grassDisplayDistance = 100;
            };

            TerrainContainer(const Config&, RenderTarget&);

            void addTerrain(const std::shared_ptr<Terrain>&);
            void removeTerrain(const Terrain&);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void prepareRendering(unsigned int, const Camera&, float) const;

        private:
            void updateAllTerrainConfig() const;
            void updateTerrainConfig(Terrain&) const;

            Config config;
            RenderTarget& renderTarget;

            std::vector<std::shared_ptr<Terrain>> terrains;
    };

}
