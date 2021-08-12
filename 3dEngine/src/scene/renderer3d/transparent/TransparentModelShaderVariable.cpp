#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>

namespace urchin {

    void TransparentModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        cameraPlanes.nearPlane = 0.1f;
        cameraPlanes.farPlane = 100.0f;

        meshRendererBuilder
                ->addUniformData(sizeof(cameraPlanes), &cameraPlanes); //binding 3
    }

    void TransparentModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer) {
        cameraPlanes.nearPlane = 0.1f;
        cameraPlanes.farPlane = 2000.0f; //TODO udpate dynamic

        meshRenderer.updateUniformData(3, &cameraPlanes);
    }

}
