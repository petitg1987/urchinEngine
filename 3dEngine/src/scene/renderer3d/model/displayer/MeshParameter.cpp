#include "MeshParameter.h"

namespace urchin
{

    MeshParameter::MeshParameter() :
        diffuseTextureUnit(-1),
        normalTextureUnit(-1),
        ambientFactorLoc(-1)
    {

    }

    /**
     * @param diffuseTextureUnit Diffuse texture unit. If -1, there is no diffuse texture.
     */
    void MeshParameter::setDiffuseTextureUnit(int diffuseTextureUnit)
    {
        this->diffuseTextureUnit = diffuseTextureUnit;
    }

    /**
     * @return Diffuse texture unit. If -1, there is no diffuse texture.
     */
    int MeshParameter::getDiffuseTextureUnit() const
    {
        return diffuseTextureUnit;
    }

    /**
     * @param normalTextureUnit Normal texture unit. If -1, there is no normal texture.
     */
    void MeshParameter::setNormalTextureUnit(int normalTextureUnit)
    {
        this->normalTextureUnit = normalTextureUnit;
    }

    /**
     * @return Normal texture unit. If -1, there is no normal texture.
     */
    int MeshParameter::getNormalTextureUnit() const
    {
        return normalTextureUnit;
    }

    /**
     * @param ambientFactorLoc Ambient factor location. If -1, there is no ambient factor.
     */
    void MeshParameter::setAmbientFactorLoc(int ambientFactorLoc)
    {
        this->ambientFactorLoc = ambientFactorLoc;
    }

    /**
     * @return Ambient factor location. If -1, there is no ambient factor.
     */
    int MeshParameter::getAmbientFactorLoc() const
    {
        return ambientFactorLoc;
    }

}
