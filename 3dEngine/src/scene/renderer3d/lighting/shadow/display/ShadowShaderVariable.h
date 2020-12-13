#ifndef URCHINENGINE_SHADOWUNIFORM_H
#define URCHINENGINE_SHADOWUNIFORM_H

#include <vector>
#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/model/displayer/CustomShaderVariable.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    /**
    * Load custom shader variable for shadow model displayer
    */
    class ShadowShaderVariable : public CustomShaderVariable {
        public:
            ShadowShaderVariable();

            void setProjectionMatricesShaderVar(const ShaderVar&);
            void setLightShadowMap(const LightShadowMap*);

            void loadCustomShaderVariables() override;

        private:
            void updateProjectionMatrices();

            ShaderVar mModelProjectionMatrixShaderVar;
            const LightShadowMap* lightShadowMap;

            std::vector<Matrix4<float>> projectionMatrices;
    };

}

#endif
