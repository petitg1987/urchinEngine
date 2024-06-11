#include <stdexcept>

#include <resources/image/Image.h>

namespace urchin {

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<unsigned char>&& texels8, bool hasTransparency) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_8_INT),
            texels8(std::move(texels8)),
            bHasTransparency(hasTransparency) {

    }

    Image::Image(unsigned int width, unsigned int height, ImageFormat format, std::vector<uint16_t>&& texels16, bool hasTransparency) :
            width(width),
            height(height),
            format(format),
            channelPrecision(ChannelPrecision::CHANNEL_16_INT),
            texels16(std::move(texels16)),
            bHasTransparency(hasTransparency) {

    }

    unsigned int Image::getWidth() const {
        return width;
    }

    unsigned int Image::getHeight() const {
        return height;
    }

    float Image::getRatio() const {
        return (float)width / (float)height;
    }

    Image::ImageFormat Image::getImageFormat() const {
        return format;
    }

    unsigned int Image::getComponentCount() const {
        if (format == ImageFormat::IMAGE_RGBA) {
            return 4;
        } else if (format == ImageFormat::IMAGE_GRAYSCALE) {
            return 1;
        }
        throw std::runtime_error("Unknown image format: " + std::to_string(format));
    }

    Image::ChannelPrecision Image::getChannelPrecision() const {
        return channelPrecision;
    }

    const std::vector<unsigned char>& Image::getTexels() const {
        if (channelPrecision != ChannelPrecision::CHANNEL_8_INT) {
            throw std::runtime_error("Channel must have 8 bits. Channel type: " + std::to_string(channelPrecision));
        }
        return texels8;
    }

    const std::vector<uint16_t>& Image::getTexels16Bits() const {
        if (channelPrecision != ChannelPrecision::CHANNEL_16_INT) {
            throw std::runtime_error("Channel must have 16 bits. Channel type: " + std::to_string(channelPrecision));
        }
        return texels16;
    }

    bool Image::hasTransparency() const {
        return bHasTransparency;
    }

    unsigned int Image::retrieveComponentsCount() const {
        if (format == Image::IMAGE_GRAYSCALE) {
            return 1;
        } else if (format == Image::IMAGE_RGBA) {
            return 4;
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format) + ".");
        }
    }

    TextureFormat Image::retrieveTextureFormat() const {
        if (format == Image::IMAGE_GRAYSCALE) {
            if (channelPrecision == Image::CHANNEL_8_INT) {
                return TextureFormat::GRAYSCALE_8_INT;
            } else if (channelPrecision == Image::CHANNEL_16_INT) {
                return TextureFormat::GRAYSCALE_16_FLOAT;
            } else {
                throw std::runtime_error("Unknown channel precision: " + std::to_string(channelPrecision));
            }
        } else if (format == Image::IMAGE_RGBA) {
            if (channelPrecision == Image::CHANNEL_8_INT) {
                return TextureFormat::RGBA_8_INT;
            } else {
                throw std::invalid_argument("Unsupported channel precision for RGBA format: " + std::to_string(channelPrecision));
            }
        } else {
            throw std::runtime_error("Unknown image format: " + std::to_string(format));
        }
    }

}
