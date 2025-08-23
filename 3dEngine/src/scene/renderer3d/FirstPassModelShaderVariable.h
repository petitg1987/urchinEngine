#pragma once

#include "scene/renderer3d/camera/Camera.h"
#include "scene/renderer3d/model/displayer/CustomModelShaderVariable.h"
#include "scene/renderer3d/lighting/light/LightManager.h"

namespace urchin {

    class FirstPassModelShaderVariable final : public CustomModelShaderVariable {
        public:
            FirstPassModelShaderVariable(const Camera&, unsigned int, unsigned int);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t, uint32_t) override;

        private:
            struct CameraInfo {
                alignas(8) Vector2<float> jitterInPixel;
            } cameraInfo;

            const Camera& camera;
            Vector2<float> jitterScale;
    };

}
