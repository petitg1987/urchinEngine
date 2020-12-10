#include <bitset>
#include "UrchinCommon.h"

#include "LoaderPNG.h"

namespace urchin {
    LoaderPNG::LoaderPNG() : Loader<Image>() {

    }

    Image* LoaderPNG::loadFromFile(const std::string& filename) {
        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
        std::vector<unsigned char> png;
        unsigned int errorLoad = lodepng::load_file(png, filenamePath);
        if (errorLoad != 0) {
            throw std::invalid_argument("Cannot load the file " + filenamePath + ": " + lodepng_error_text(errorLoad));
        }

        unsigned int width, height;
        lodepng::State state;
        state.info_raw.bitdepth=16;
        std::vector<unsigned char> pixelsRGBA16bits;
        unsigned int errorRead = lodepng::decode(pixelsRGBA16bits, width, height, state, png);
        if (errorRead != 0) {
            throw std::invalid_argument("Cannot read the file " + filenamePath + ": " + lodepng_error_text(errorLoad));
        }

        unsigned int bitDepth = state.info_png.color.bitdepth;
        LodePNGColorType colorType = state.info_png.color.colortype;
        if (colorType == LodePNGColorType::LCT_GREY) {
            if (bitDepth == 8) {
                return new Image(width, height, Image::IMAGE_GRAYSCALE, extract8BitsChannels(pixelsRGBA16bits, 1));
            } else if (bitDepth == 16) {
                return new Image(width, height, Image::IMAGE_GRAYSCALE, extract16BitsChannels(pixelsRGBA16bits, 1));
            } else {
                throw std::invalid_argument("Unsupported number of bits for PNG image (grayscale): " + std::to_string(bitDepth));
            }
        } else if (colorType == LodePNGColorType::LCT_RGB) {
            if (bitDepth == 8) {
                return new Image(width, height, Image::IMAGE_RGB, extract8BitsChannels(pixelsRGBA16bits, 7));
            } else {
                throw std::invalid_argument("Unsupported number of bits for PNG image (RGB): " + std::to_string(bitDepth));
            }
        } else if (colorType == LodePNGColorType::LCT_RGBA) {
            if (bitDepth == 8) {
                return new Image(width, height, Image::IMAGE_RGBA, extract8BitsChannels(pixelsRGBA16bits, 15));
            } else {
                throw std::invalid_argument("Unsupported number of bits for PNG image (RGBA): " + std::to_string(bitDepth));
            }
        } else {
            throw std::invalid_argument("Unsupported color type for PNG image: " + std::to_string(colorType));
        }
    }

    /**
     * @param channelsMask Channel to extract where bit 0: R, bit 1: G, bit 2: B,  bit 3: A
     */
    std::vector<unsigned char> LoaderPNG::extract8BitsChannels(const std::vector<unsigned char>& pixelsRGBA16bits, unsigned int channelsMask) {
        std::vector<unsigned char> pixels;
        size_t nbChannels = std::bitset<8>(channelsMask).count();
        pixels.reserve((pixelsRGBA16bits.size()/(4*2))*nbChannels);

        for (std::size_t i=7; i<pixelsRGBA16bits.size(); i+=8) {
            if (channelsMask & 1u) { //red
                pixels.push_back(pixelsRGBA16bits[i-6]);
            }
            if (channelsMask & 2u) { //green
                pixels.push_back(pixelsRGBA16bits[i-4]);
            }
            if (channelsMask & 4u) { //blue
                pixels.push_back(pixelsRGBA16bits[i-2]);
            }
            if (channelsMask & 8u) { //alpha
                pixels.push_back(pixelsRGBA16bits[i-0]);
            }
        }

        return pixels;
    }

    /**
     * @param channelsMask Channel to extract where bit 0: R, bit 1: G, bit 2: B,  bit 3: A
     */
    std::vector<uint16_t> LoaderPNG::extract16BitsChannels(const std::vector<unsigned char>& pixelsRGBA16bits, unsigned int channelsMask) {
        std::vector<uint16_t> pixels;
        size_t nbChannels = std::bitset<8>(channelsMask).count();
        pixels.reserve((pixelsRGBA16bits.size()/(4*2))*nbChannels);

        for (std::size_t i = 7; i < pixelsRGBA16bits.size(); i+=8) {
            if (channelsMask & 1u) { //red
                pixels.push_back((uint16_t)((uint16_t)(pixelsRGBA16bits[i - 7] << 8u) | (uint16_t)(pixelsRGBA16bits[i - 6])));
            }
            if (channelsMask & 2u) { //green
                pixels.push_back((uint16_t)((uint16_t)(pixelsRGBA16bits[i - 5] << 8u) | (uint16_t)(pixelsRGBA16bits[i - 4])));
            }
            if (channelsMask & 4u) { //blue
                pixels.push_back((uint16_t)((uint16_t)(pixelsRGBA16bits[i - 3] << 8u) | (uint16_t)(pixelsRGBA16bits[i - 2])));
            }
            if (channelsMask & 8u) { //alpha
                pixels.push_back((uint16_t)((uint16_t)(pixelsRGBA16bits[i - 1] << 8u) | (uint16_t)(pixelsRGBA16bits[i - 0])));
            }
        }

        return pixels;
    }
}
