#pragma once

#include <UrchinCommon.h>

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/model/displayer/CustomModelShaderVariable.h"

namespace urchin {

    class ModelShadowSpotShaderVariable final : public CustomModelShaderVariable {
        public:
            explicit ModelShadowSpotShaderVariable(const LightShadowMap*);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t, uint32_t) override;

        private:
            void refreshShaderVariables();

            const LightShadowMap* lightShadowMap;

            struct ShadowData {
                alignas(16) Matrix4<float> lightProjectionViewMatrix;
            } shadowData;
    };

}
