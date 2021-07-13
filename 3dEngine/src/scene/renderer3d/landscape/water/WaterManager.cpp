#include <algorithm>

#include <scene/renderer3d/landscape/water/WaterManager.h>

namespace urchin {

    WaterManager::WaterManager(RenderTarget& renderTarget) :
            renderTarget(renderTarget) {

    }

    void WaterManager::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto water : waters) {
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::addWater(Water* water) {
        if (water) {
            waters.push_back(water);

            water->initialize(renderTarget);
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::removeWater(Water* water) {
        if (water) {
            waters.erase(std::remove(waters.begin(), waters.end(), water), waters.end());
            delete water;
        }
    }

    void WaterManager::prepareRendering(const Camera& camera, FogManager* fogManager, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "waterPreRender");

        for (const auto water : waters) {
            water->prepareRendering(camera, fogManager, dt);
        }
    }
}
