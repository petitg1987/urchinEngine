#include <algorithm>

#include <scene/renderer3d/landscape/terrain/TerrainManager.h>



namespace urchin {
    TerrainManager::TerrainManager(RenderTarget& renderTarget) :
            renderTarget(renderTarget),
            config({}) {

    }

    void TerrainManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto& terrain : terrains) {
            terrain->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void TerrainManager::addTerrain(const std::shared_ptr<Terrain>& terrain) {
        if (terrain) {
            terrains.push_back(terrain);

            terrain->initialize(renderTarget);
            terrain->onCameraProjectionUpdate(projectionMatrix);
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainManager::removeTerrain(const Terrain& terrain) {
        auto itFind = std::find_if(terrains.begin(), terrains.end(), [&terrain](const auto& o){return o.get() == &terrain;});
        if (itFind != terrains.end()) {
            terrains.erase(itFind);
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
        for (const auto& terrain : terrains) {
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainManager::updateTerrainConfig(Terrain& terrain) const {
        terrain.getGrass().setGrassDisplayDistance(config.grassDisplayDistance);
    }

    void TerrainManager::prepareRendering(const Camera& camera, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "terPreRender");

        for (const auto& terrain : terrains) {
            terrain->prepareRendering(camera, dt);
        }
    }
}
