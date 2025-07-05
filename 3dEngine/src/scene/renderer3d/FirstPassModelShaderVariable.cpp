#include <cstring>

#include <scene/renderer3d/FirstPassModelShaderVariable.h>

namespace urchin {

    FirstPassModelShaderVariable::FirstPassModelShaderVariable(const Camera& camera, unsigned int renderingSceneWidth, unsigned int renderingSceneHeight) :
            camera(camera),
            renderingSceneWidth((float)renderingSceneWidth),
            renderingSceneHeight((float)renderingSceneHeight) {
        std::memset((void *)&cameraInfo, 0, sizeof(cameraInfo));

        cameraInfo.jitterInPixel = Vector2(0.0f, 0.0f);
    }

    void FirstPassModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t) {
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(cameraInfo), &cameraInfo);
    }

    void FirstPassModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t) {
        constexpr float NDC_SPACE_TO_UV_COORDS_SCALE = 0.5f;
        cameraInfo.jitterInPixel = camera.getAppliedJitter() * Vector2(renderingSceneWidth * NDC_SPACE_TO_UV_COORDS_SCALE, renderingSceneHeight * NDC_SPACE_TO_UV_COORDS_SCALE);

        meshRenderer.updateUniformData(uniformBinding1, &cameraInfo);
    }

}
