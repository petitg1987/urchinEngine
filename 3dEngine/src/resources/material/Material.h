#pragma once

#include <vector>

#include <resources/Resource.h>
#include <resources/image/Image.h>

namespace urchin {

    class Material : public Resource {
        public:
            Material(bool, std::shared_ptr<Texture>, std::shared_ptr<Texture>, bool, float, float);
            ~Material() override = default;

            static constexpr float MAX_EMISSIVE_FACTOR = 20.0f;

            bool isRepeatTextures() const;
            const std::shared_ptr<Texture>& getDiffuseTexture() const;
            const std::shared_ptr<Texture>& getNormalTexture() const;

            bool hasTransparency() const;

            float getEmissiveFactor() const;
            float getAmbientFactor() const;

        private:
            bool repeatTextures;
            std::shared_ptr<Texture> diffuseTexture;
            std::shared_ptr<Texture> normalTexture;

            bool bHasTransparency;

            float emissiveFactor;
            float ambientFactor;
    };

}
