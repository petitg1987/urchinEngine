#ifndef URCHINENGINE_SHADOWUNIFORM_H
#define URCHINENGINE_SHADOWUNIFORM_H

#include <vector>
#include "UrchinCommon.h"

#include "scene/renderer3d/lighting/shadow/data/ShadowData.h"
#include "scene/renderer3d/model/displayer/CustomUniform.h"
#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    /**
    * Load custom uniform for shadow model displayer
    */
    class ShadowUniform : public CustomUniform {
        public:
            ShadowUniform();

            void setProjectionMatricesShaderVar(const ShaderVar &);
            void setUniformData(const ShadowData *);

            void loadCustomUniforms(std::unique_ptr<Shader> &) override;

        private:
            void updateProjectionMatrices();

            ShaderVar mModelProjectionMatrixShaderVar;
            const ShadowData *shadowData;

            std::vector<Matrix4<float>> projectionMatrices;
    };

}

#endif
