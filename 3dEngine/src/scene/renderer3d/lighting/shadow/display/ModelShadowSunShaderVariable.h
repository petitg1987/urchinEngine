#pragma once

#include <array>
#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/ShadowManager.h>
#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class ModelShadowSunShaderVariable final : public CustomModelShaderVariable {
        public:
            explicit ModelShadowSunShaderVariable(const LightShadowMap*);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t) override;

        private:
            void refreshShaderVariables();

            const LightShadowMap* lightShadowMap;

            struct ShadowData {
                alignas(16) std::array<Matrix4<float>, ShadowManager::SPLIT_SHADOW_MAPS_SHADER_LIMIT> lightProjectionViewMatrices;
            } shadowData;
    };

}
