#include "resources/material/Material.h"

#include <utility>

namespace urchin {

    Material::Material(std::shared_ptr<Texture> diffuseTexture, std::shared_ptr<Texture> normalTexture, bool repeatableTextures, float ambientFactor) :
            diffuseTexture(std::move(diffuseTexture)),
            normalTexture(std::move(normalTexture)),
            repeatableTextures(repeatableTextures),
            ambientFactor(ambientFactor) {
        buildDefaultTextures();
    }

    void Material::buildDefaultTextures() {
        if (!diffuseTexture) {
            auto *defaultDiffuseImage = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({255, 20, 147}));
            diffuseTexture = defaultDiffuseImage->createTexture(false);
        }

        if (!normalTexture) {
            auto *defaultNormalImage = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({127, 127, 255}));
            normalTexture = defaultNormalImage->createTexture(false);
        }
    }

    const std::shared_ptr<Texture> &Material::getDiffuseTexture() const {
        return diffuseTexture;
    }

    const std::shared_ptr<Texture> &Material::getNormalTexture() const {
        return normalTexture;
    }

    bool Material::isRepeatableTextures() const {
        return repeatableTextures;
    }

    float Material::getAmbientFactor() const {
        return ambientFactor;
    }

}
