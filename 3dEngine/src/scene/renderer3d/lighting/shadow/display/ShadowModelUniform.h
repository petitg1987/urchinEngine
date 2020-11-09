#ifndef URCHINENGINE_SHADOWMODELUNIFORM_H
#define URCHINENGINE_SHADOWMODELUNIFORM_H

#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
#include "scene/renderer3d/model/Model.h"
#include "scene/renderer3d/model/displayer/CustomModelUniform.h"
#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    /**
    * Load custom model uniform for shadow model displayer
    */
    class ShadowModelUniform : public CustomModelUniform {
        public:
            ShadowModelUniform();

            void setLayersToUpdateShaderVar(const ShaderVar &);
            void setModelUniformData(const ShadowData *);

            void loadCustomUniforms(const Model *, std::unique_ptr<Shader> &) override;

        private:
            ShaderVar layersToUpdateShaderVar;
            const ShadowData *shadowData;
    };

}

#endif
