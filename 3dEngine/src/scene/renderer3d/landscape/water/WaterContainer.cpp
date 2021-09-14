#include <algorithm>

#include <scene/renderer3d/landscape/water/WaterContainer.h>

namespace urchin {

    WaterContainer::WaterContainer(RenderTarget& renderTarget) :
            renderTarget(renderTarget) {

    }

    void WaterContainer::onCameraProjectionUpdate(const Camera& camera) {
        this->projectionMatrix = camera.getProjectionMatrix();
        for (const auto& water : waters) {
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterContainer::addWater(const std::shared_ptr<Water>& water) {
        if (water) {
            ScopeProfiler sp(Profiler::graphic(), "addWater");

            waters.push_back(water);

            water->initialize(renderTarget);
            water->onCameraProjectionUpdate(projectionMatrix);
        }
    }

    void WaterContainer::removeWater(const Water& water) {
        auto itFind = std::find_if(waters.begin(), waters.end(), [&water](const auto& o){return o.get() == &water;});
        if (itFind != waters.end()) {
            waters.erase(itFind);
        }
    }

    void WaterContainer::prepareRendering(unsigned int& renderingOrder, const Camera& camera, FogContainer& fogContainer, float dt) const {
        ScopeProfiler sp(Profiler::graphic(), "waterPreRender");

        for (const auto& water : waters) {
            water->prepareRendering(renderingOrder, camera, fogContainer, dt);
        }
    }
}
