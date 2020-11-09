#ifndef URCHINENGINE_MESHPARAMETER_H
#define URCHINENGINE_MESHPARAMETER_H

#include "UrchinCommon.h"

#include "graphic/shader/model/ShaderVar.h"

namespace urchin {

    class MeshParameter {
        public:
            MeshParameter();

            void setDiffuseTextureUnit(int);
            int getDiffuseTextureUnit() const;

            void setNormalTextureUnit(int);
            int getNormalTextureUnit() const;

            void setAmbientFactorShaderVar(const ShaderVar &);
            const ShaderVar &getAmbientFactorShaderVar() const;

        private:
            int diffuseTextureUnit;
            int normalTextureUnit;
            ShaderVar ambientFactorShaderVar;
    };

}

#endif
