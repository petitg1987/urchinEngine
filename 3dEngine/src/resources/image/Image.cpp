#include <stdexcept>

#include "resources/image/Image.h"

namespace urchin {

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<unsigned char>&& texels8) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_8),
            texels8(texels8) {

    }

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<uint16_t>&& texels16) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_16),
            texels16(texels16) {

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

    const std::vector<unsigned char>& Image::getTexels() const {
        if (channelPrecision != ChannelPrecision::CHANNEL_8) {
            throw std::runtime_error("Channel must have 8 bits. Channel type: " + std::to_string(channelPrecision));
        }

        return texels8;
    }

    const std::vector<uint16_t>& Image::getTexels16Bits() const {
        if (channelPrecision != ChannelPrecision::CHANNEL_16) {
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

    TextureFormat Image::retrieveTextureFormat() const {
        if (format == Image::IMAGE_GRAYSCALE) {
            if (channelPrecision == Image::CHANNEL_8) {
                return TextureFormat::GRAYSCALE_8_INT;
            } else if (channelPrecision == Image::CHANNEL_16) {
                return TextureFormat::GRAYSCALE_16_FLOAT;
            } else {
                throw std::runtime_error("Unknown channel precision: " + std::to_string(channelPrecision));
            }
        } else if (format == Image::IMAGE_RGB) {
            if (channelPrecision == Image::CHANNEL_8) {
                return TextureFormat::RGB_8_INT;
            } else {
                throw std::invalid_argument("Unsupported channel precision for RGB format: " + std::to_string(channelPrecision));
            }
        } else if (format == Image::IMAGE_RGBA) {
            if (channelPrecision == Image::CHANNEL_8) {
                return TextureFormat::RGBA_8_INT;
            } else {
                throw std::invalid_argument("Unsupported channel precision for RGBA format: " + std::to_string(channelPrecision));
            }
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format));
        }
    }

    std::shared_ptr<Texture> Image::createTexture(bool generateMipMap) {
        std::shared_ptr<Texture> texture;
        if(channelPrecision == Image::CHANNEL_8) {
            texture = Texture::build(width, height, retrieveTextureFormat(), &texels8[0]);
        }else if(channelPrecision == Image::CHANNEL_16) {
            texture = Texture::build(width, height, retrieveTextureFormat(), &texels16[0]);
        }

        if (generateMipMap) {
            texture->generateMipmap();
        }

        return texture;
    }
}
