#ifndef URCHINENGINETEST_TEXTURE_H
#define URCHINENGINETEST_TEXTURE_H

#include <vector>
#include <memory>

#include "graphic/texture/model/TextureType.h"
#include "graphic/texture/model/TextureFormat.h"

namespace urchin {

    class Texture {
        public:
            Texture(TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void* > &);
            ~Texture();

            static std::shared_ptr<Texture> build(unsigned int, unsigned int, TextureFormat, const void *);
            static std::shared_ptr<Texture> buildArray(unsigned int, unsigned int, unsigned int, TextureFormat, const void *);
            static std::shared_ptr<Texture> buildCubeMap(unsigned int, unsigned int, TextureFormat, const std::vector<const void *> &);

            void generateMipmap() const;

            TextureType getTextureType() const;
            unsigned int getTextureId() const;

        private:
            unsigned int textureTypeToGlTextureType(TextureType) const;
            unsigned int textureFormatToGlInternalFormat(TextureFormat) const;
            unsigned int textureFormatToGlFormat(TextureFormat) const;
            unsigned int textureFormatToGlPixelType(TextureFormat) const;

            TextureType textureType;
            unsigned int width, height, layer;
            TextureFormat format;
            unsigned int textureId;
    };

}

#endif
