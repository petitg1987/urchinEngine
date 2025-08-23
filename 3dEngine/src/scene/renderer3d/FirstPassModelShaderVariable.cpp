#include <cstring>

#include "scene/renderer3d/FirstPassModelShaderVariable.h"

namespace urchin {

    FirstPassModelShaderVariable::FirstPassModelShaderVariable(const Camera& camera, unsigned int renderingSceneWidth, unsigned int renderingSceneHeight) :
            camera(camera) {
        std::memset((void*)&cameraInfo, 0, sizeof(cameraInfo));

        constexpr float NDC_SPACE_TO_UV_COORDS_SCALE = 0.5f;
        jitterScale = Vector2((float)renderingSceneWidth * NDC_SPACE_TO_UV_COORDS_SCALE, (float)renderingSceneHeight * NDC_SPACE_TO_UV_COORDS_SCALE);
        cameraInfo.jitterInPixel = Vector2(0.0f, 0.0f);
    }

    void FirstPassModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t, uint32_t) {
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(cameraInfo), &cameraInfo);
    }

    void FirstPassModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t, uint32_t) {
        cameraInfo.jitterInPixel = camera.getAppliedJitter() * jitterScale;
        meshRenderer.updateUniformData(uniformBinding1, &cameraInfo);
    }

}
