#ifndef URCHINENGINE_SHADOWMODELUNIFORM_H
#define URCHINENGINE_SHADOWMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
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
            ShadowModelShaderVariable();

            void setLayersToUpdateShaderVar(const ShaderVar&);
            void setShadowData(const ShadowData*);

            void loadCustomShaderVariables(const Model*) override;

        private:
            ShaderVar layersToUpdateShaderVar;
            const ShadowData *shadowData;
    };

}

#endif
