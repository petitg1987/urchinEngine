#include <resources/material/Material.h>

namespace urchin {

    Material::Material(bool repeatTextures, std::shared_ptr<Texture> diffuseTexture, std::shared_ptr<Texture> normalTexture,
                       bool hasTransparency, float emissiveFactor, float ambientFactor) :
            repeatTextures(repeatTextures),
            diffuseTexture(std::move(diffuseTexture)),
            normalTexture(std::move(normalTexture)),
            bHasTransparency(hasTransparency),
            emissiveFactor(emissiveFactor),
            ambientFactor(ambientFactor) {
        if (!this->diffuseTexture) {
            throw std::runtime_error("Diffuse texture is missing on a material");
        }
        if (!this->normalTexture) {
            Image defaultNormalImage(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({127, 127, 255, 255}), false);
            this->normalTexture = defaultNormalImage.createTexture(false);
        }
    }

    bool Material::isRepeatTextures() const {
        return repeatTextures;
    }

    const std::shared_ptr<Texture>& Material::getDiffuseTexture() const {
        return diffuseTexture;
    }

    const std::shared_ptr<Texture>& Material::getNormalTexture() const {
        return normalTexture;
    }

    bool Material::hasTransparency() const {
        return bHasTransparency;
    }

    float Material::getEmissiveFactor() const {
        return emissiveFactor;
    }

    float Material::getAmbientFactor() const {
        return ambientFactor;
    }

}
