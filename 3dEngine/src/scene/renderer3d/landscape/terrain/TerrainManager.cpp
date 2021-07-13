#include <algorithm>

#include <scene/renderer3d/landscape/terrain/TerrainManager.h>



namespace urchin {
    TerrainManager::TerrainManager(RenderTarget& renderTarget) :
            renderTarget(renderTarget),
            config({}) {

    }

    void TerrainManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto terrain : terrains) {
            terrain->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void TerrainManager::addTerrain(Terrain* terrain) {
        if (terrain) {
            terrains.push_back(terrain);

            terrain->initialize(renderTarget);
            terrain->onCameraProjectionUpdate(projectionMatrix);
            updateTerrainConfig(terrain);
        }
    }

    void TerrainManager::removeTerrain(Terrain* terrain) {
        if (terrain) {
            terrains.erase(std::remove(terrains.begin(), terrains.end(), terrain), terrains.end());
            delete terrain;
        }
    }

    void TerrainManager::updateConfig(const Config& config) {
        if (this->config.grassDisplayDistance != config.grassDisplayDistance) {
            this->config = config;

            updateAllTerrainConfig();
        }
    }

    const TerrainManager::Config& TerrainManager::getConfig() const {
        return config;
    }

    void TerrainManager::updateAllTerrainConfig() {
        for (const auto terrain : terrains) {
            updateTerrainConfig(terrain);
        }
    }

    void TerrainManager::updateTerrainConfig(Terrain* terrain) const {
        terrain->getGrass()->setGrassDisplayDistance(config.grassDisplayDistance);
    }

    void TerrainManager::prepareRendering(const Camera& camera, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "terPreRender");

        for (const auto terrain : terrains) {
            terrain->prepareRendering(camera, dt);
        }
    }
}
