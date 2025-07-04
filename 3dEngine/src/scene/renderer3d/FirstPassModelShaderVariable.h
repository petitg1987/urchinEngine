#pragma once

#include <scene/renderer3d/camera/Camera.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>
#include <scene/renderer3d/lighting/light/LightManager.h>

namespace urchin {

    class FirstPassModelShaderVariable final : public CustomModelShaderVariable { //TODO rename in ModelShaderVariable ?
        public:
            explicit FirstPassModelShaderVariable(const Camera&);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t) override;

        private:
            struct {
                alignas(8) Vector2<float> values;
            } jitter;
            const Camera& camera;
    };

}
