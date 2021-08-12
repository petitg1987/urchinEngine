#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/lighting/shadow/light/LightShadowMap.h>
#include <scene/renderer3d/model/Model.h>
#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>
#include <graphic/render/shader/model/Shader.h>

namespace urchin {

    /**
    * Load custom model shader variables for shadow model displayer
    */
    class ShadowModelShaderVariable : public CustomModelShaderVariable {
        public:
            explicit ShadowModelShaderVariable(const LightShadowMap*);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&) override;
            void loadCustomShaderVariables(GenericRenderer&) override;

        private:
            void refreshShaderVariables();

            const LightShadowMap* lightShadowMap;

            struct {
                alignas(4) int layerToUpdate;
                alignas(16) std::array<Matrix4<float>, 10> projectionMatrices;
            } shadowData;
    };

}
