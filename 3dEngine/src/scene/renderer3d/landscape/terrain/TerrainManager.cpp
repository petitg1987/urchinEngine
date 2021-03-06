#include <algorithm>

#include "TerrainManager.h"

#define DEFAULT_GRASS_DISPLAY_DISTANCE 100

namespace urchin {
    TerrainManager::TerrainManager(std::shared_ptr<RenderTarget> renderTarget) :
            renderTarget(std::move(renderTarget)),
            grassDisplayDistance(DEFAULT_GRASS_DISPLAY_DISTANCE) {

    }

    void TerrainManager::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();
        for (const auto terrain : terrains) {
            terrain->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void TerrainManager::addTerrain(Terrain* terrain) {
        if (terrain) {
            terrains.push_back(terrain);

            terrain->onCameraProjectionUpdate(projectionMatrix);
            updateWithConfig();
        }
    }

    void TerrainManager::removeTerrain(Terrain* terrain) {
        if (terrain) {
            terrains.erase(std::remove(terrains.begin(), terrains.end(), terrain), terrains.end());
            delete terrain;
        }
    }

    void TerrainManager::setGrassDisplayDistance(float grassDisplayDistance) {
        this->grassDisplayDistance = grassDisplayDistance;

        updateWithConfig();
    }

    void TerrainManager::updateWithConfig() {
        for (const auto terrain : terrains) {
            terrain->getGrass()->setGrassDisplayDistance(grassDisplayDistance);
        }
    }

    void TerrainManager::display(const Camera* camera, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "terrainDisplay");

        for (const auto terrain : terrains) {
            terrain->display(camera, dt);
        }
    }
}
