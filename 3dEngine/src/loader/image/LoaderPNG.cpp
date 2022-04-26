#include <bitset>
#include <UrchinCommon.h>

#include <loader/image/LoaderPNG.h>

namespace urchin {

    std::shared_ptr<Image> LoaderPNG::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        std::vector<unsigned char> png;
        unsigned int errorLoad = lodepng::load_file(png, filename);
        if (errorLoad != 0) {
            throw std::invalid_argument("Cannot load the file " + filename + ": " + lodepng_error_text(errorLoad));
        }

        //see lodepng.cpp#lodepng_inspect:
        unsigned char bitDepth = png[24];
        auto colorType = (LodePNGColorType)png[25];

        unsigned int width;
        unsigned int height;
        lodepng::State state = {};

        if (colorType == LodePNGColorType::LCT_RGB || colorType == LodePNGColorType::LCT_RGBA) {
            if (bitDepth == 8) {
                state.info_raw.bitdepth = 8;
                state.info_raw.colortype = LCT_RGBA;
                std::vector<unsigned char> pixels8Bits = decode(filename, state, png, width, height);
                bool hasTransparency = false;
                if (colorType == LodePNGColorType::LCT_RGBA) {
                    hasTransparency = isPixelsHaveTransparency(pixels8Bits);
                }
                return std::make_shared<Image>(width, height, Image::IMAGE_RGBA, std::move(pixels8Bits), hasTransparency);
            } else {
                throw std::invalid_argument("Unsupported number of bits for PNG image (grayscale): " + std::to_string(bitDepth));
            }
        } else if (colorType == LodePNGColorType::LCT_GREY) {
            if (bitDepth == 8) {
                state.info_raw.bitdepth = 8;
                state.info_raw.colortype = LCT_GREY;
                std::vector<unsigned char> pixels8Bits = decode(filename, state, png, width, height);
                return std::make_shared<Image>(width, height, Image::IMAGE_GRAYSCALE, std::move(pixels8Bits), false);
            } else if (bitDepth == 16) {
                state.info_raw.bitdepth = 16;
                state.info_raw.colortype = LCT_GREY;
                std::vector<uint16_t> pixels16Bits = to16Bits(decode(filename, state, png, width, height));
                return std::make_shared<Image>(width, height, Image::IMAGE_GRAYSCALE, std::move(pixels16Bits), false);
            } else {
                throw std::invalid_argument("Unsupported number of bits for PNG image (grayscale): " + std::to_string(bitDepth));
            }
        } else {
            throw std::invalid_argument("Unsupported color type for PNG image: " + std::to_string(colorType));
        }
    }

    /**
     * @param width [out] Image width
     * @param height [out] Image height
     */
    std::vector<unsigned char> LoaderPNG::decode(const std::string& filenamePath, lodepng::State& state, const std::vector<unsigned char>& png, unsigned int& width, unsigned int& height) const {
        std::vector<unsigned char> pixels;
        unsigned int errorRead = lodepng::decode(pixels, width, height, state, png);
        if (errorRead != 0) {
            throw std::invalid_argument("Cannot decode the PNG image " + filenamePath + ": " + lodepng_error_text(errorRead));
        }
        return pixels;
    }

    bool LoaderPNG::isPixelsHaveTransparency(const std::vector<unsigned char>& pixelsRgba8Bits) const {
        assert(pixelsRgba8Bits.size() % 4 == 0);
        for (std::size_t i = 3; i < pixelsRgba8Bits.size(); i += 4) {
            if (pixelsRgba8Bits[i] < 255) {
                return true;
            }
        }
        return false;
    }

    std::vector<uint16_t> LoaderPNG::to16Bits(const std::vector<unsigned char>& pixelsGrey16bits) const {
        std::vector<uint16_t> pixels;
        pixels.resize(pixelsGrey16bits.size() / 2);

        for (std::size_t i = 0; i < pixels.size(); ++i) {
            pixels[i] = (uint16_t)((uint16_t)(pixelsGrey16bits[i * 2] << 8u) | (uint16_t)(pixelsGrey16bits[i * 2 + 1]));
        }

        return pixels;
    }

}
