#include <stdexcept>

#include "resources/image/Image.h"

namespace urchin {

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<unsigned char> &&texels8) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_8),
            texels8(texels8),
            isTexture(false),
            textureID(0) {

    }

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<uint16_t> &&texels16) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_16),
            texels16(texels16),
            isTexture(false),
            textureID(0) {

    }

    Image::~Image() {
        if (isTexture) {
            glDeleteTextures(1, &textureID);
        }
    }

    unsigned int Image::getWidth() const {
        return width;
    }

    unsigned int Image::getHeight() const {
        return height;
    }

    Image::ImageFormat Image::getImageFormat() const {
        return format;
    }

    Image::ChannelPrecision Image::getChannelPrecision() const {
        return channelPrecision;
    }

    const std::vector<unsigned char> &Image::getTexels() const {
        if (isTexture) {
            throw std::runtime_error("The image \"" + getName() + "\" was transformed into a texture, you cannot get the texels.");
        }

        if (channelPrecision!=ChannelPrecision::CHANNEL_8) {
            throw std::runtime_error("Channel must have 8 bits. Channel type: " + std::to_string(channelPrecision));
        }

        return texels8;
    }

    const std::vector<uint16_t> &Image::getTexels16Bits() const {
        if (isTexture) {
            throw std::runtime_error("The image \"" + getName() + "\" was transformed into a texture, you cannot get the texels.");
        }

        if (channelPrecision!=ChannelPrecision::CHANNEL_16) {
            throw std::runtime_error("Channel must have 16 bits. Channel type: " + std::to_string(channelPrecision));
        }

        return texels16;
    }

    unsigned int Image::retrieveComponentsCount() const {
        if (format==Image::IMAGE_GRAYSCALE) {
            return 1;
        } else if (format==Image::IMAGE_RGB) {
            return 3;
        } else if (format==Image::IMAGE_RGBA) {
            return 4;
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
        }
    }

    GLint Image::retrieveInternalFormat() const {
        if (format==Image::IMAGE_GRAYSCALE) {
            return GL_RED;
        } else if (format==Image::IMAGE_RGB) {
            return GL_RGB;
        } else if (format==Image::IMAGE_RGBA) {
            return GL_RGBA;
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
        }
    }

    GLenum Image::retrieveFormat() const {
        if (format==Image::IMAGE_GRAYSCALE) {
            return GL_RED;
        } else if (format==Image::IMAGE_RGB) {
            return GL_RGB;
        } else if (format==Image::IMAGE_RGBA) {
            return GL_RGBA;
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
        }
    }

    unsigned int Image::toTexture(bool generateMipMaps) {
        if (isTexture) {
            return textureID;
        }

        if (channelPrecision!=ChannelPrecision::CHANNEL_8) {
            throw std::runtime_error("Channel bit not supported to convert into texture: " + std::to_string(channelPrecision));
        }

        glGenTextures(1, &textureID);
        glBindTexture (GL_TEXTURE_2D, textureID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, retrieveInternalFormat(), width, height, 0, retrieveFormat(), GL_UNSIGNED_BYTE, &texels8[0]);
        if (generateMipMaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        texels8.clear(); //the API has its own copy
        isTexture = true;

        return textureID;
    }

    unsigned int Image::getTextureID() const {
        if (!isTexture) {
            throw std::runtime_error("The image \"" + getName() + "\" isn't a texture. Use Image::toTexture() for transform the image into texture.");
        }
        return textureID;
    }
}
