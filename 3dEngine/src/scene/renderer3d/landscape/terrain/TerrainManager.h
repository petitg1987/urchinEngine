#ifndef URCHINENGINE_TERRAINMANAGER_H
#define URCHINENGINE_TERRAINMANAGER_H

#include <UrchinCommon.h>

#include <graphic/render/target/RenderTarget.h>
#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/landscape/terrain/Terrain.h>

#define DEFAULT_GRASS_DISPLAY_DISTANCE 100

namespace urchin {

    class TerrainManager {
        public:
            struct Config {
                float grassDisplayDistance = DEFAULT_GRASS_DISPLAY_DISTANCE;
            };

            explicit TerrainManager(std::shared_ptr<RenderTarget>);

            void onCameraProjectionUpdate(const Camera*);

            void addTerrain(Terrain*);
            void removeTerrain(Terrain*);

            void updateConfig(const Config&);
            const Config& getConfig() const;

            void prepareRendering(const Camera*, float) const;

        private:
            void updateAllTerrainConfig();
            void updateTerrainConfig(Terrain*) const;

            std::shared_ptr<RenderTarget> renderTarget;

            std::vector<Terrain*> terrains;

            Matrix4<float> projectionMatrix;

            Config config;
    };

}

#endif
