#include <cstring>

#include <scene/renderer3d/landscape/fog/FogContainer.h>

namespace urchin {

    FogContainer::FogContainer() :
            fogData({}) {
        std::memset((void*)&fogData, 0, sizeof(fogData));
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

    void FogContainer::setupDeferredSecondPassRenderer(const std::shared_ptr<GenericRendererBuilder>& deferredSecondPassRendererBuilder, uint32_t fogUniformBinding) const {
        deferredSecondPassRendererBuilder->addUniformData(fogUniformBinding, sizeof(fogData), &fogData);
    }

    void FogContainer::loadFog(GenericRenderer& deferredSecondPassRenderer, uint32_t fogUniformBinding) {
        const Fog* activeFog = getActiveFog();
        bool hasFog = activeFog != nullptr;

        fogData.hasFog = hasFog;
        fogData.density = hasFog ? activeFog->getDensity() : 0.0f;
        fogData.gradient = hasFog ? activeFog->getGradient() : 0.0f;
        fogData.maxHeight = hasFog ? activeFog->getMaxHeight() : 0.0f;
        fogData.color = hasFog ? activeFog->getColor() : Vector3(0.0f, 0.0f, 0.0f);

        deferredSecondPassRenderer.updateUniformData(fogUniformBinding, &fogData);
    }

}
