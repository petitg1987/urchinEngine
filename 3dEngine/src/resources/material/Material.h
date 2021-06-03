#ifndef URCHINENGINE_MATERIAL_H
#define URCHINENGINE_MATERIAL_H

#include <vector>

#include <resources/Resource.h>
#include <resources/image/Image.h>

namespace urchin {

    class Material : public Resource {
        public:
            Material(std::shared_ptr<Texture>, std::shared_ptr<Texture>, bool, float);
            ~Material() override = default;

            const std::shared_ptr<Texture>& getDiffuseTexture() const;
            const std::shared_ptr<Texture>& getNormalTexture() const;

            bool isRepeatableTextures() const;

            float getAmbientFactor() const;

        private:
            void buildDefaultTextures();

            std::shared_ptr<Texture> diffuseTexture;
            std::shared_ptr<Texture> normalTexture;

            bool repeatableTextures;

            float ambientFactor;
    };

}

#endif
