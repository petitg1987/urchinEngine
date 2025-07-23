#pragma once

#include "resources/Resource.h"
#include "resources/material/UvScale.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class MaterialBuilder;

    class Material final : public Resource {
        public:
            explicit Material(const MaterialBuilder&);
            ~Material() override = default;

            static constexpr float MAX_EMISSIVE_FACTOR = 20.0f;

            const std::shared_ptr<Texture>& getAlbedoTexture() const;
            bool hasTransparency() const;

            bool repeatTextures() const;
            const UvScale& getUvScale() const;
            const std::shared_ptr<Texture>& getNormalTexture() const;
            const std::shared_ptr<Texture>& getRoughnessTexture() const;
            const std::shared_ptr<Texture>& getMetalnessTexture() const;

            float getEmissiveFactor() const;
            float getAmbientFactor() const;

            bool isDepthTestEnabled() const;
            bool isDepthWriteEnabled() const;

        private:
            std::shared_ptr<Texture> albedoTexture;
            bool bHasTransparency;

            bool bRepeatTextures;
            UvScale uvScale;
            std::shared_ptr<Texture> normalTexture;
            std::shared_ptr<Texture> roughnessTexture;
            std::shared_ptr<Texture> metalnessTexture;

            float emissiveFactor;
            float ambientFactor;

            bool depthTestEnabled;
            bool depthWriteEnabled;
    };

}
