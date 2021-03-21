#ifndef URCHINENGINE_SHADOWMODELUNIFORM_H
#define URCHINENGINE_SHADOWMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/light/LightShadowMap.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/CustomModelShaderVariable.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    /**
    * Load custom model shader variables for shadow model displayer
    */
    class ShadowModelShaderVariable : public CustomModelShaderVariable {
        public:
            ShadowModelShaderVariable(const LightShadowMap*, const ShaderVar&, const ShaderVar&);

            void setupMeshRenderer(const std::unique_ptr<GenericRendererBuilder>&) override;
            void loadCustomShaderVariables(const std::unique_ptr<GenericRenderer>&) override;

        private:
            void refreshShaderVariables();

            ShaderVar mModelProjectionMatrixShaderVar, layersToUpdateShaderVar;
            const LightShadowMap* lightShadowMap;

            std::vector<Matrix4<float>> projectionMatrices;
            int layerToUpdate;
    };

}

#endif
