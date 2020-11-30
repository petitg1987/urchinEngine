#include <algorithm>

#include "WaterManager.h"

namespace urchin {

    WaterManager::WaterManager(const RenderTarget* renderTarget) :
            renderTarget(renderTarget) {
    }

    void WaterManager::onCameraProjectionUpdate(const Camera* camera) {
        this->projectionMatrix = camera->getProjectionMatrix();
        for (const auto water : waters) {
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::addWater(Water* water) {
        if (water) {
            waters.push_back(water);

            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterManager::removeWater(Water* water) {
        if (water) {
            waters.erase(std::remove(waters.begin(), waters.end(), water), waters.end());
            delete water;
        }
    }

    void WaterManager::display(const Camera* camera, FogManager* fogManager, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "waterDisplay");

        for (const auto water : waters) {
            water->display(renderTarget, camera, fogManager, dt);
        }
    }
}
