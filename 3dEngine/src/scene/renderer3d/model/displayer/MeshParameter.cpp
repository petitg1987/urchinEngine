#include "MeshParameter.h"

namespace urchin {

    MeshParameter::MeshParameter() :
            bNeedRenderTextures(true) {

    }

    void MeshParameter::setNeedRenderTextures(bool bNeedRenderTextures) {
        this->bNeedRenderTextures = bNeedRenderTextures;
    }

    bool MeshParameter::needRenderTextures() const {
        return bNeedRenderTextures;
    }

    void MeshParameter::setAmbientFactorShaderVar(const ShaderVar& ambientFactorShaderVar) {
        this->ambientFactorShaderVar = ambientFactorShaderVar;
    }

    const ShaderVar& MeshParameter::getAmbientFactorShaderVar() const {
        return ambientFactorShaderVar;
    }

}
