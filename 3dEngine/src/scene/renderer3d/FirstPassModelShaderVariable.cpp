#include <cstring>

#include <scene/renderer3d/FirstPassModelShaderVariable.h>

namespace urchin {

    FirstPassModelShaderVariable::FirstPassModelShaderVariable(const Camera& camera) :
            camera(camera) {
        std::memset((void *)&jitter, 0, sizeof(jitter));

        jitter.values = Vector2(0.0f, 0.0f);
    }

    void FirstPassModelShaderVariable::setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>& meshRendererBuilder, uint32_t uniformBinding1, uint32_t) {
        assert(meshRendererBuilder->getUniformData().size() == 2);
        meshRendererBuilder->addUniformData(uniformBinding1, sizeof(jitter), &jitter);
    }

    void FirstPassModelShaderVariable::loadCustomShaderVariables(GenericRenderer& meshRenderer, uint32_t uniformBinding1, uint32_t) {
        constexpr float ndcSpaceToUvCoordinatesScale = 0.5f;
        jitter.values = camera.getAppliedJitter() * Vector2(2560.0f * ndcSpaceToUvCoordinatesScale, 1440.0f * ndcSpaceToUvCoordinatesScale); //TODO remove hardcoded value
        meshRenderer.updateUniformData(uniformBinding1, &jitter);
    }

}
