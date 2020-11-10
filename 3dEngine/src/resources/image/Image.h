#ifndef URCHINENGINE_CIMAGE_H
#define URCHINENGINE_CIMAGE_H

#include <GL/glew.h>
#include <vector>

#include "resources/Resource.h"

namespace urchin {

    class Image : public Resource {
        public:
            enum ImageFormat {
                IMAGE_RGB,
                IMAGE_RGBA,
                IMAGE_GRAYSCALE
            };
            enum ChannelPrecision {
                CHANNEL_8,
                CHANNEL_16
            };

            Image(unsigned int, unsigned int, ImageFormat, std::vector<unsigned char> &&);
            Image(unsigned int, unsigned int, ImageFormat, std::vector<uint16_t> &&);
            ~Image() override;

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            ImageFormat getImageFormat() const;
            ChannelPrecision getChannelPrecision() const;
            const std::vector<unsigned char> &getTexels() const;
            const std::vector<uint16_t> &getTexels16Bits() const;

            unsigned int retrieveComponentsCount() const;
            GLint retrieveInternalFormat() const;
            GLenum retrieveFormat() const;

            unsigned int toTexture(bool, bool, bool);
            unsigned int getTextureID() const;

        private:
            unsigned int width;
            unsigned int height;
            ImageFormat format;
            ChannelPrecision channelPrecision;
            std::vector<unsigned char> texels8; //8 bits
            std::vector<uint16_t> texels16; //16 bits

            bool isTexture;
            unsigned int textureID;
    };

}

#endif
