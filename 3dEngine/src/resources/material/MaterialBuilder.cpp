#include <utility>

#include "resources/material/Material.h"
#include "resources/material/MaterialBuilder.h"

namespace urchin {

    //static
    unsigned long MaterialBuilder::nextId = 0;

    MaterialBuilder::MaterialBuilder(std::string materialName, std::shared_ptr<Texture> albedoTexture) :
            materialName(std::move(materialName)),
            mAlbedoTexture(std::move(albedoTexture)),
            mRepeatTextures(false),
            mUvScale(UvScale(UvScaleType::NONE)),
            mEmissiveFactor(0.0f),
            mAmbientFactor(0.5f),
            mDepthTestEnabled(true),
            mDepthWriteEnabled(true) {

    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::create(std::string materialName, std::shared_ptr<Texture> albedoTexture) {
        if (!albedoTexture) {
            throw std::runtime_error("Albedo texture is mandatory to build a material");
        }

        return std::shared_ptr<MaterialBuilder>(new MaterialBuilder(std::move(materialName), std::move(albedoTexture)));
    }

    const std::string& MaterialBuilder::getMaterialName() const {
        return materialName;
    }

    const std::shared_ptr<Texture>& MaterialBuilder::getAlbedoTexture() const {
        return mAlbedoTexture;
    }

    bool MaterialBuilder::hasTransparency() const {
        return mAlbedoTexture->hasTransparency();
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::enableRepeatTextures() {
        this->mRepeatTextures = true;
        return shared_from_this();
    }

    bool MaterialBuilder::repeatTextures() const {
        return mRepeatTextures;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::normalTexture(std::shared_ptr<Texture> normalTexture) {
        this->mNormalTexture = std::move(normalTexture);
        return shared_from_this();
    }

    const std::shared_ptr<Texture>& MaterialBuilder::getNormalTexture() const {
        return mNormalTexture;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::roughnessTexture(std::shared_ptr<Texture> roughnessTexture) {
        this->mRoughnessTexture = std::move(roughnessTexture);
        return shared_from_this();
    }

    const std::shared_ptr<Texture>& MaterialBuilder::getRoughnessTexture() const {
        return mRoughnessTexture;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::metalnessTexture(std::shared_ptr<Texture> metalnessTexture) {
        this->mMetalnessTexture = std::move(metalnessTexture);
        return shared_from_this();
    }

    const std::shared_ptr<Texture>& MaterialBuilder::getMetalnessTexture() const {
        return mMetalnessTexture;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::uvScale(const UvScale& uvScale) {
        this->mUvScale = uvScale;
        return shared_from_this();
    }

    const UvScale& MaterialBuilder::getUvScale() const {
        return mUvScale;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::emissiveFactor(float emissiveFactor) {
        if (emissiveFactor > Material::MAX_EMISSIVE_FACTOR) {
            throw std::runtime_error("Emissive factor (" + std::to_string(emissiveFactor) + ") is above the maximum (" + std::to_string(Material::MAX_EMISSIVE_FACTOR) + ")");
        }

        this->mEmissiveFactor = emissiveFactor;
        return shared_from_this();
    }

    float MaterialBuilder::getEmissiveFactor() const {
        return mEmissiveFactor;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::ambientFactor(float ambientFactor) {
        this->mAmbientFactor = ambientFactor;
        return shared_from_this();
    }

    float MaterialBuilder::getAmbientFactor() const {
        return mAmbientFactor;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::disableDepthTest() {
        this->mDepthTestEnabled = false;
        return shared_from_this();
    }

    bool MaterialBuilder::isDepthTestEnabled() const {
        return mDepthTestEnabled;
    }

    std::shared_ptr<MaterialBuilder> MaterialBuilder::disableDepthWrite() {
        this->mDepthWriteEnabled = false;
        return shared_from_this();
    }

    bool MaterialBuilder::isDepthWriteEnabled() const {
        return mDepthWriteEnabled;
    }

    std::unique_ptr<Material> MaterialBuilder::build() {
        auto material = std::make_unique<Material>(*this);
        material->setId("_" + std::to_string(nextId++));
        material->setName(materialName);
        return material;
    }

}
