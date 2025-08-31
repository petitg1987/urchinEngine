#include "scene/renderer3d/landscape/terrain/TerrainContainer.h"

namespace urchin {

    TerrainContainer::TerrainContainer(const Config& config, RenderTarget& renderTarget) :
            config(config),
            renderTarget(renderTarget) {

    }

    void TerrainContainer::addTerrain(const std::shared_ptr<Terrain>& terrain) {
        if (terrain) {
            ScopeProfiler sp(Profiler::graphic(), "addTerrain");
            terrains.push_back(terrain);

            terrain->initialize(renderTarget);
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainContainer::removeTerrain(const Terrain& terrain) {
        auto itFind = std::ranges::find_if(terrains, [&terrain](const auto& o){return o.get() == &terrain;});
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

    void TerrainContainer::updateAllTerrainConfig() const {
        for (const auto& terrain : terrains) {
            updateTerrainConfig(*terrain);
        }
    }

    void TerrainContainer::updateTerrainConfig(Terrain& /*terrain*/) const {
        //TODO terrain.getGrass().setGrassDisplayDistance(config.grassDisplayDistance);
    }

    void TerrainContainer::prepareRendering(unsigned int& renderingOrder, const Camera& camera, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "terPreRender");

        for (const auto& terrain : terrains) {
            terrain->prepareRendering(renderingOrder, camera, dt);
            renderingOrder++;
        }
    }
}
