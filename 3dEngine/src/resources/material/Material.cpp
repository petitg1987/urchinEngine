#include <resources/material/Material.h>
#include <resources/material/MaterialBuilder.h>

namespace urchin {

    Material::Material(const MaterialBuilder& materialBuilder) :
            diffuseTexture(materialBuilder.getDiffuseTexture()),
            bHasTransparency(materialBuilder.hasTransparency()),
            bRepeatTextures(materialBuilder.repeatTextures()),
            uvScale(materialBuilder.getUvScale()),
            normalTexture(materialBuilder.getNormalTexture()),
            roughnessTexture(materialBuilder.getRoughnessTexture()),
            metalnessTexture(materialBuilder.getMetalnessTexture()),
            emissiveFactor(materialBuilder.getEmissiveFactor()),
            ambientFactor(materialBuilder.getAmbientFactor()),
            depthTestEnabled(materialBuilder.isDepthTestEnabled()),
            depthWriteEnabled(materialBuilder.isDepthWriteEnabled()) {
        if (!this->normalTexture) {
            std::vector<unsigned char> defaultNormalColor({127, 127, 255, 255});
            this->normalTexture = Texture::build(materialBuilder.getMaterialName() + " - default normal", 1, 1, TextureFormat::RGBA_8_INT, defaultNormalColor.data());
        }
        if (!this->roughnessTexture) {
            std::vector<unsigned char> defaultRoughnessValue({255});
            this->roughnessTexture = Texture::build(materialBuilder.getMaterialName() + " - default roughness", 1, 1, TextureFormat::GRAYSCALE_8_INT, defaultRoughnessValue.data());
        }
        if (!this->metalnessTexture) {
            std::vector<unsigned char> defaultMetalnessValue({0});
            this->metalnessTexture = Texture::build(materialBuilder.getMaterialName() + " - default metalness", 1, 1, TextureFormat::GRAYSCALE_8_INT, defaultMetalnessValue.data());
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

    const std::shared_ptr<Texture>& Material::getRoughnessTexture() const {
        return roughnessTexture;
    }

    const std::shared_ptr<Texture>& Material::getMetalnessTexture() const {
        return metalnessTexture;
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
