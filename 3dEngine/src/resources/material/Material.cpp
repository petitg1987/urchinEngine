#include <resources/material/Material.h>

namespace urchin {

    Material::Material(std::shared_ptr<Texture> diffuseTexture, std::shared_ptr<Texture> normalTexture, bool hasTransparency,
                       bool repeatableTextures, float ambientFactor) :
            diffuseTexture(std::move(diffuseTexture)),
            normalTexture(std::move(normalTexture)),
            bHasTransparency(hasTransparency),
            repeatableTextures(repeatableTextures),
            ambientFactor(ambientFactor) {
        buildDefaultTextures();
    }

    void Material::buildDefaultTextures() {
        if (!diffuseTexture) {
            Image defaultDiffuseImage(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 20, 147, 255}), false);
            diffuseTexture = defaultDiffuseImage.createTexture(false);
        }

        if (!normalTexture) {
            Image defaultNormalImage(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({127, 127, 255, 255}), false);
            normalTexture = defaultNormalImage.createTexture(false);
        }
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

    bool Material::isRepeatableTextures() const {
        return repeatableTextures;
    }

    float Material::getAmbientFactor() const {
        return ambientFactor;
    }

}
