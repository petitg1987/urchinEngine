#ifndef URCHINENGINE_FOGMANAGER_H
#define URCHINENGINE_FOGMANAGER_H

#include <stack>
#include <memory>

#include "scene/renderer3d/landscape/fog/Fog.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class FogManager {
        public:
            void pushFog(const std::shared_ptr<Fog> &);
            void popFog();
            std::shared_ptr<const Fog> getCurrentFog() const;

            void loadUniformLocationFor(const std::shared_ptr<Shader> &);
            void loadFog();

        private:
            std::stack<std::shared_ptr<Fog>> fogs;

            std::shared_ptr<Shader> lightingShader;
            ShaderVar hasFogShaderVar, fogDensityShaderVar, fogGradientShaderVar, fogColorShaderVar, fogMaxHeightShaderVar;
    };

}

#endif
