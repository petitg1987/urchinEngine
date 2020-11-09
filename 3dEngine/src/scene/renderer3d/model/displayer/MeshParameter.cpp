#include "MeshParameter.h"

namespace urchin {

    MeshParameter::MeshParameter() :
        diffuseTextureUnit(-1),
        normalTextureUnit(-1) {

    }

    /**
     * @param diffuseTextureUnit Diffuse texture unit. If -1, there is no diffuse texture.
     */
    void MeshParameter::setDiffuseTextureUnit(int diffuseTextureUnit) {
        this->diffuseTextureUnit = diffuseTextureUnit;
    }

    /**
     * @return Diffuse texture unit. If -1, there is no diffuse texture.
     */
    int MeshParameter::getDiffuseTextureUnit() const {
        return diffuseTextureUnit;
    }

    /**
     * @param normalTextureUnit Normal texture unit. If -1, there is no normal texture.
     */
    void MeshParameter::setNormalTextureUnit(int normalTextureUnit) {
        this->normalTextureUnit = normalTextureUnit;
    }

    /**
     * @return Normal texture unit. If -1, there is no normal texture.
     */
    int MeshParameter::getNormalTextureUnit() const {
        return normalTextureUnit;
    }

    void MeshParameter::setAmbientFactorShaderVar(const ShaderVar &ambientFactorShaderVar) {
        this->ambientFactorShaderVar = ambientFactorShaderVar;
    }

    const ShaderVar &MeshParameter::getAmbientFactorShaderVar() const {
        return ambientFactorShaderVar;
    }

}
