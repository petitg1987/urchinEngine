#include <scene/renderer3d/landscape/fog/FogContainer.h>

namespace urchin {

    FogContainer::FogContainer() :
            fogData({}) {

    }

    void FogContainer::pushFog(const Fog* fog) {
        fogs.push(fog);
    }

    void FogContainer::popFog() {
        if (!fogs.empty()) {
            fogs.pop();
        }
    }

    const Fog* FogContainer::getActiveFog() const {
        if (fogs.empty()) {
            return nullptr;
        }
        return fogs.top();
    }

    void FogContainer::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& lightingRendererBuilder, uint32_t fogUniformBinding) const {
        lightingRendererBuilder->addUniformData(fogUniformBinding, sizeof(fogData), &fogData);
    }

    void FogContainer::loadFog(GenericRenderer& lightingRenderer, uint32_t fogUniformBinding) {
        fogData = {};
        fogData.hasFog = !fogs.empty();
        if (!fogs.empty()) {
            fogData.density = fogs.top()->getDensity();
            fogData.gradient = fogs.top()->getGradient();
            fogData.maxHeight = fogs.top()->getMaxHeight();
            fogData.color = fogs.top()->getColor();
        }

        lightingRenderer.updateUniformData(fogUniformBinding, &fogData);
    }

}
