#include <algorithm>

#include <scene/renderer3d/landscape/terrain/TerrainContainer.h>

namespace urchin {

    TerrainContainer::TerrainContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget),
            config({}) {

    }

    void TerrainContainer::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto& terrain : terrains) {
            terrain->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void TerrainContainer::addTerrain(const std::shared_ptr<Terrain>& terrain) {
        if (terrain) {
            terrains.push_back(terrain);

            terrain->initialize(renderTarget);
            terrain->onCameraProjectionUpdate(projectionMatrix);
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainContainer::removeTerrain(const Terrain& terrain) {
        auto itFind = std::find_if(terrains.begin(), terrains.end(), [&terrain](const auto& o){return o.get() == &terrain;});
        if (itFind != terrains.end()) {
            terrains.erase(itFind);
        }
    }

    void TerrainContainer::updateConfig(const Config& config) {
        if (this->config.grassDisplayDistance != config.grassDisplayDistance) {
            this->config = config;

            updateAllTerrainConfig();
        }
    }

    const TerrainContainer::Config& TerrainContainer::getConfig() const {
        return config;
    }

    void TerrainContainer::updateAllTerrainConfig() {
        for (const auto& terrain : terrains) {
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainContainer::updateTerrainConfig(Terrain& terrain) const {
        terrain.getGrass().setGrassDisplayDistance(config.grassDisplayDistance);
    }

    void TerrainContainer::prepareRendering(unsigned int& renderingOrder, const Camera& camera, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "terPreRender");

        for (const auto& terrain : terrains) {
            terrain->prepareRendering(renderingOrder, camera, dt);
        }
    }
}
