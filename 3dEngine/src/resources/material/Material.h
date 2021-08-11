#pragma once

#include <vector>

#include <resources/Resource.h>
#include <resources/image/Image.h>

namespace urchin {

    class Material : public Resource {
        public:
            Material(std::shared_ptr<Texture>, std::shared_ptr<Texture>, bool, bool, float);
            ~Material() override = default;

            const std::shared_ptr<Texture>& getDiffuseTexture() const;
            const std::shared_ptr<Texture>& getNormalTexture() const;

            bool hasTransparency() const;
            bool isRepeatableTextures() const;

            float getAmbientFactor() const;

        private:
            void buildDefaultTextures();

            std::shared_ptr<Texture> diffuseTexture;
            std::shared_ptr<Texture> normalTexture;

            bool bHasTransparency;
            bool repeatableTextures;

            float ambientFactor;
    };

}
