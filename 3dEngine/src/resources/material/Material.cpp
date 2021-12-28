#include <resources/material/Material.h>
#include <resources/material/MaterialBuilder.h>

namespace urchin {

    Material::Material(const MaterialBuilder& materialBuilder) :
            diffuseTexture(materialBuilder.getDiffuseTexture()),
            bHasTransparency(materialBuilder.hasTransparency()),
            bRepeatTextures(materialBuilder.repeatTextures()),
            uvScale(materialBuilder.getUvScale()),
            normalTexture(materialBuilder.getNormalTexture()),
            emissiveFactor(materialBuilder.getEmissiveFactor()),
            ambientFactor(materialBuilder.getAmbientFactor()),
            depthTestEnabled(materialBuilder.isDepthTestEnabled()),
            depthWriteEnabled(materialBuilder.isDepthWriteEnabled()) {
        if (!this->normalTexture) {
            Image defaultNormalImage(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({127, 127, 255, 255}), false);
            this->normalTexture = defaultNormalImage.createTexture(false);
        }
    }

    const std::shared_ptr<Texture>& Material::getDiffuseTexture() const {
        return diffuseTexture;
    }

    bool Material::hasTransparency() const {
        return bHasTransparency;
    }

    bool Material::repeatTextures() const {
        return bRepeatTextures;
    }

    const UvScale& Material::getUvScale() const {
        return uvScale;
    }

    const std::shared_ptr<Texture>& Material::getNormalTexture() const {
        return normalTexture;
    }

    float Material::getEmissiveFactor() const {
        return emissiveFactor;
    }

    float Material::getAmbientFactor() const {
        return ambientFactor;
    }

    bool Material::isDepthTestEnabled() const {
        return depthTestEnabled;
    }
    
    bool Material::isDepthWriteEnabled() const {
        return depthWriteEnabled;
    }

}
