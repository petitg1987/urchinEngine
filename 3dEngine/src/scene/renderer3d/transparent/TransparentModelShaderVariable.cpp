#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>

namespace urchin {

    TransparentModelShaderVariable::TransparentModelShaderVariable(float nearPlane, float fatPlane, LightManager& lightManager) :
            cameraPlanes({}),
            lightManager(lightManager) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = fatPlane;
    }

    void TransparentModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        assert(meshRendererBuilder->getUniformData().size() == 3);
        meshRendererBuilder
                ->addUniformData(sizeof(cameraPlanes), &cameraPlanes); //binding 3
        lightManager.setupLightingRenderer(meshRendererBuilder); //binding 4
    }

    void TransparentModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer) {
        lightManager.loadVisibleLights(meshRenderer, 4);
    }

}
