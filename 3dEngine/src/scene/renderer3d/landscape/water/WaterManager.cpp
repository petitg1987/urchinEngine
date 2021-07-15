#include <algorithm>

#include <scene/renderer3d/landscape/water/WaterManager.h>

namespace urchin {

    WaterManager::WaterManager(RenderTarget& renderTarget) :
            renderTarget(renderTarget) {

    }

    void WaterManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto& water : waters) {
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::addWater(const std::shared_ptr<Water>& water) {
        if (water) {
            waters.push_back(water);

            water->initialize(renderTarget);
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::removeWater(const Water& water) {
        auto itFind = std::find_if(waters.begin(), waters.end(), [&water](const auto& o){return o.get() == &water;});
        if (itFind != waters.end()) {
            waters.erase(itFind);
        }
    }

    void WaterManager::prepareRendering(const Camera& camera, FogManager* fogManager, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "waterPreRender");

        for (const auto& water : waters) {
            water->prepareRendering(camera, fogManager, dt);
        }
    }
}
