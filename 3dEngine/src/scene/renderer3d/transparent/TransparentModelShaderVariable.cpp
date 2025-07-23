#include <cstring>

#include "scene/renderer3d/transparent/TransparentModelShaderVariable.h"

namespace urchin {

    TransparentModelShaderVariable::TransparentModelShaderVariable(float nearPlane, float fatPlane, LightManager& lightManager) :
            cameraPlanes({}),
            lightManager(lightManager) {
        std::memset(&cameraPlanes, 0, sizeof(cameraPlanes));

        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = fatPlane;
    }

    void TransparentModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t uniformBinding2) {
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(cameraPlanes), &cameraPlanes);
        lightManager.setupDeferredSecondPassRenderer(meshRendererBuilder, uniformBinding2);
    }

    void TransparentModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t, uint32_t uniformBinding2) {
        lightManager.loadVisibleLights(meshRenderer, uniformBinding2);
    }

}
