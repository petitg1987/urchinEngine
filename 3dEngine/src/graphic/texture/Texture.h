#ifndef URCHINENGINETEST_TEXTURE_H
#define URCHINENGINETEST_TEXTURE_H

#include <vector>

#include "graphic/texture/model/TextureType.h"
#include "graphic/texture/model/TextureFormat.h"

namespace urchin {

    class Texture {
        public:
            static Texture build2d(unsigned int, unsigned int, TextureFormat, const void *);
            static Texture build3d(unsigned int, unsigned int, unsigned int, TextureFormat, const void *);
            static Texture buildCubeMap(unsigned int, unsigned int, TextureFormat, const std::vector<const void *> &);

            void generateMipmap() const;

        private:
            Texture(TextureType textureType, unsigned int, unsigned int, unsigned int, TextureFormat, const std::vector<const void* > &);
            ~Texture();

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
