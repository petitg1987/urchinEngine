#pragma once

#include <memory>

#include "resources/material/UvScale.h"
#include "resources/material/Material.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class MaterialBuilder : public std::enable_shared_from_this<MaterialBuilder> {
        public:
            static std::shared_ptr<MaterialBuilder> create(std::string, std::shared_ptr<Texture>, bool);

            const std::string& getMaterialName() const;

            const std::shared_ptr<Texture>& getAlbedoTexture() const;
            bool hasTransparency() const;

            std::shared_ptr<MaterialBuilder> enableRepeatTextures();
            bool repeatTextures() const;

            std::shared_ptr<MaterialBuilder> normalTexture(std::shared_ptr<Texture>);
            const std::shared_ptr<Texture>& getNormalTexture() const;

            std::shared_ptr<MaterialBuilder> roughnessTexture(std::shared_ptr<Texture>);
            const std::shared_ptr<Texture>& getRoughnessTexture() const;

            std::shared_ptr<MaterialBuilder> metalnessTexture(std::shared_ptr<Texture>);
            const std::shared_ptr<Texture>& getMetalnessTexture() const;

            std::shared_ptr<MaterialBuilder> uvScale(const UvScale&);
            const UvScale& getUvScale() const;

            std::shared_ptr<MaterialBuilder> emissiveFactor(float);
            float getEmissiveFactor() const;

            std::shared_ptr<MaterialBuilder> ambientFactor(float);
            float getAmbientFactor() const;

            std::shared_ptr<MaterialBuilder> disableDepthTest();
            bool isDepthTestEnabled() const;
            std::shared_ptr<MaterialBuilder> disableDepthWrite();
            bool isDepthWriteEnabled() const;

            std::unique_ptr<Material> build();

        private:
            MaterialBuilder(std::string, std::shared_ptr<Texture>, bool);

            static unsigned long nextId;

            std::string materialName;
            std::shared_ptr<Texture> mAlbedoTexture;
            bool mHasTransparency;

            bool mRepeatTextures;
            UvScale mUvScale;
            std::shared_ptr<Texture> mNormalTexture;
            std::shared_ptr<Texture> mRoughnessTexture;
            std::shared_ptr<Texture> mMetalnessTexture;

            float mEmissiveFactor;
            float mAmbientFactor;

            bool mDepthTestEnabled;
            bool mDepthWriteEnabled;
    };

}
