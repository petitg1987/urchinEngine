#include "MeshParameter.h"

namespace urchin {

    void MeshParameter::setAmbientFactorShaderVar(const ShaderVar& ambientFactorShaderVar) {
        this->ambientFactorShaderVar = ambientFactorShaderVar;
    }

    const ShaderVar& MeshParameter::getAmbientFactorShaderVar() const {
        return ambientFactorShaderVar;
    }

}
