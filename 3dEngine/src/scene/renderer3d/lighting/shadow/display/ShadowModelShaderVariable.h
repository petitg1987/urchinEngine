#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    /**
    * Load custom model shader variables for shadow model displayer
    */
    class ShadowModelShaderVariable final : public CustomModelShaderVariable {
        public:
            explicit ShadowModelShaderVariable(const LightShadowMap*);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t) override;

        private:
            void refreshShaderVariables();

            const LightShadowMap* lightShadowMap;

            struct {
                alignas(16) std::array<Matrix4<float>, 10> projectionMatrices;
            } shadowData;
    };

}
