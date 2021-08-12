#pragma once

#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class TransparentModelShaderVariable : public CustomModelShaderVariable {
        public:
            TransparentModelShaderVariable(float, float);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&) override;
            void loadCustomShaderVariables(GenericRenderer&) override;

        private:
            struct {
                alignas(4) float nearPlane;
                alignas(4) float farPlane;
            } cameraPlanes;
    };

}
