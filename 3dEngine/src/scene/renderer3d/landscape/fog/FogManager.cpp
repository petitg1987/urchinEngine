#include "FogManager.h"

namespace urchin {

    FogManager::FogManager() :
        fogData({}) {

    }

    void FogManager::pushFog(const std::shared_ptr<Fog>& fog) {
        fogs.push(fog);
    }

    void FogManager::popFog() {
        if (!fogs.empty()) {
            fogs.pop();
        }
    }

    std::shared_ptr<const Fog> FogManager::getActiveFog() const {
        if (fogs.empty()) {
            return nullptr;
        }
        return fogs.top();
    }

    void FogManager::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder) {
        lightingRendererBuilder->addUniformData(sizeof(fogData), &fogData); //binding 6
    }

    void FogManager::loadFog(const std::shared_ptr<GenericRenderer>& lightingRenderer) {
        fogData = {};
        fogData.hasFog = !fogs.empty();
        if (!fogs.empty()) {
            fogData.density = fogs.top()->getDensity();
            fogData.gradient = fogs.top()->getGradient();
            fogData.maxHeight = fogs.top()->getMaxHeight();
            fogData.color = Vector4<float>(fogs.top()->getColor(), 1.0);
        }

        lightingRenderer->updateUniformData(6, &fogData);
    }

}
