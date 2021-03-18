#ifndef URCHINENGINE_FOGMANAGER_H
#define URCHINENGINE_FOGMANAGER_H

#include <stack>
#include <memory>

#include "scene/renderer3d/landscape/fog/Fog.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/GenericRendererBuilder.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class FogManager {
        public:
            FogManager();

            void pushFog(const std::shared_ptr<Fog>&);
            void popFog();
            std::shared_ptr<const Fog> getActiveFog() const;

            void initiateShaderVariables(const std::shared_ptr<Shader>&);
            void setupLightingRenderer(const std::unique_ptr<GenericRendererBuilder>&);

            void loadFog(const std::unique_ptr<GenericRenderer>&);

        private:
            std::stack<std::shared_ptr<Fog>> fogs;
            ShaderVar hasFogShaderVar, fogDensityShaderVar, fogGradientShaderVar, fogColorShaderVar, fogMaxHeightShaderVar;
            struct {
                alignas(4) bool hasFog;
                alignas(4) float density;
                alignas(4) float gradient;
                alignas(4) float maxHeight;
                alignas(16) Vector4<float> color;
            } fogData;
    };

}

#endif
