#include "resources/material/Material.h"
#include "resources/material/MaterialBuilder.h"
#include "resources/ResourceRetriever.h"
#include "loader/texture/LoaderTexture.h"
namespace urchin {

    Material::Material(const MaterialBuilder& materialBuilder) :
            albedoTexture(materialBuilder.getAlbedoTexture()),
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
            this->normalTexture = ResourceRetriever::instance().getResource<Texture>(LoaderTexture::DEFAULT_TEXTURE_FILENAME, {{"textureType", "normal"}});
        }
        if (!this->roughnessTexture) {
            this->roughnessTexture = ResourceRetriever::instance().getResource<Texture>(LoaderTexture::DEFAULT_TEXTURE_FILENAME, {{"textureType", "roughness"}});
        }
        if (!this->metalnessTexture) {
            this->metalnessTexture = ResourceRetriever::instance().getResource<Texture>(LoaderTexture::DEFAULT_TEXTURE_FILENAME, {{"textureType", "metalness"}});
        }
    }

    const std::shared_ptr<Texture>& Material::getAlbedoTexture() const {
        return albedoTexture;
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
