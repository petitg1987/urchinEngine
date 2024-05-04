#pragma once

#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>
#include <scene/renderer3d/lighting/light/LightManager.h>

namespace urchin {

    class TransparentModelShaderVariable final : public CustomModelShaderVariable {
        public:
            TransparentModelShaderVariable(float, float, LightManager&);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t) override;

        private:
            struct {
                alignas(4) float nearPlane;
                alignas(4) float farPlane;
            } cameraPlanes;
            LightManager& lightManager;
    };

}
