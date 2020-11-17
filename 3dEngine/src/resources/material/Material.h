#ifndef URCHINENGINE_MATERIAL_H
#define URCHINENGINE_MATERIAL_H

#include <vector>

#include "resources/Resource.h"
#include "resources/image/Image.h"

namespace urchin {

    class Material : public Resource {
        public:
            Material(Image *, Image *, bool, float);
            ~Material() override;

            bool hasDiffuseTexture() const;
            const Image *getDiffuseTexture() const;

            bool hasNormalTexture() const;
            const Image *getNormalTexture() const;

            bool isRepeatableTextures() const;

            float getAmbientFactor() const;

            std::vector<const Image *> getTextures() const;

        private:
            void buildDefaultTextures();

            bool bHasDiffuseTexture;
            Image *diffuseTexture;

            bool bHasNormalTexture;
            Image *normalTexture;

            bool repeatableTextures;

            float ambientFactor;
    };

}

#endif
