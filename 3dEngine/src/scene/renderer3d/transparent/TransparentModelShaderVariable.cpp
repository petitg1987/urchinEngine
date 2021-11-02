#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>

namespace urchin {

    TransparentModelShaderVariable::TransparentModelShaderVariable(float nearPlane, float fatPlane, LightManager& lightManager) :
            cameraPlanes({}),
            lightManager(lightManager) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = fatPlane;
    }

    void TransparentModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        assert(meshRendererBuilder->getUniformData().size() == 2);
        meshRendererBuilder
                ->addUniformData(sizeof(cameraPlanes), &cameraPlanes); //binding 2
        lightManager.setupLightingRenderer(meshRendererBuilder); //binding 3
    }

    void TransparentModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer) {
        lightManager.loadVisibleLights(meshRenderer, 3);
    }

}
