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

    void FogContainer::setupLightingRenderer(const std::shared_ptr<GenericRendererBuilder>& deferredSecondPassRendererBuilder, uint32_t fogUniformBinding) const {
        deferredSecondPassRendererBuilder->addUniformData(fogUniformBinding, sizeof(fogData), &fogData);
    }

    void FogContainer::loadFog(GenericRenderer& deferredSecondPassRenderer, uint32_t fogUniformBinding) {
        fogData = {};
        fogData.hasFog = !fogs.empty();
        if (!fogs.empty()) {
            fogData.density = fogs.top()->getDensity();
            fogData.gradient = fogs.top()->getGradient();
            fogData.maxHeight = fogs.top()->getMaxHeight();
            fogData.color = fogs.top()->getColor();
        }

        deferredSecondPassRenderer.updateUniformData(fogUniformBinding, &fogData);
    }

}
