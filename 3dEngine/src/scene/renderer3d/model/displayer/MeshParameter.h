#ifndef URCHINENGINE_MESHPARAMETER_H
#define URCHINENGINE_MESHPARAMETER_H

#include "UrchinCommon.h"

#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class MeshParameter {
        public:
            MeshParameter();

            void setNeedRenderTextures(bool);
            bool needRenderTextures() const;

            void setAmbientFactorShaderVar(const ShaderVar&);
            const ShaderVar& getAmbientFactorShaderVar() const;

        private:
            bool bNeedRenderTextures;
            ShaderVar ambientFactorShaderVar;
    };

}

#endif
