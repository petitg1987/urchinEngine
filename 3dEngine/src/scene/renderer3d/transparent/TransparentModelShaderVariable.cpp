#include <scene/renderer3d/transparent/TransparentModelShaderVariable.h>

namespace urchin {

    TransparentModelShaderVariable::TransparentModelShaderVariable(float nearPlane, float fatPlane) :
            cameraPlanes({}) {
        cameraPlanes.nearPlane = nearPlane;
        cameraPlanes.farPlane = fatPlane;
    }

    void TransparentModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder) {
        meshRendererBuilder
                ->addUniformData(sizeof(cameraPlanes), &cameraPlanes); //binding 3
    }

    void TransparentModelShaderVariable::loadCustomShaderVariables(GenericRenderer&) {
        //nothing to update
    }

}
