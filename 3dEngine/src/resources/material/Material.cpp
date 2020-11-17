#include "resources/material/Material.h"

namespace urchin {

    Material::Material(Image *diffuseTexture, Image *normalTexture, bool repeatableTextures, float ambientFactor) :
            bHasDiffuseTexture(diffuseTexture!=nullptr),
            diffuseTexture(diffuseTexture),
            bHasNormalTexture(normalTexture!=nullptr),
            normalTexture(normalTexture),
            repeatableTextures(repeatableTextures),
            ambientFactor(ambientFactor) {
        buildDefaultTextures();
    }

    Material::~Material() {
        diffuseTexture->release();
        normalTexture->release();
    }

    void Material::buildDefaultTextures() {
        if (!hasDiffuseTexture()) {
            this->diffuseTexture = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({255, 20, 147}));
            this->diffuseTexture->toTexture(false, false, true);
        }

        if (!hasNormalTexture()) {
            this->normalTexture = new Image(1, 1, Image::IMAGE_RGB, std::vector<unsigned char>({127, 127, 255}));
            this->normalTexture->toTexture(false, false, true);
        }
    }

    bool Material::hasDiffuseTexture() const {
        return bHasDiffuseTexture;
    }

    const Image *Material::getDiffuseTexture() const {
        return diffuseTexture;
    }

    bool Material::hasNormalTexture() const {
        return bHasNormalTexture;
    }

    const Image *Material::getNormalTexture() const {
        return normalTexture;
    }

    bool Material::isRepeatableTextures() const {
        return repeatableTextures;
    }

    float Material::getAmbientFactor() const {
        return ambientFactor;
    }

    std::vector<const Image *> Material::getTextures() const {
        std::vector<const Image *> textures = {getDiffuseTexture(), getNormalTexture()};

        return textures;
    }

}
