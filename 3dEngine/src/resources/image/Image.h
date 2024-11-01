#pragma once

#include <vector>
#include <memory>

#include <resources/Resource.h>
#include <graphics/texture/TextureFormat.h>

namespace urchin {

    class Image final : public Resource {
        public:
            enum ImageFormat {
                IMAGE_RGBA,
                IMAGE_GRAYSCALE
            };
            enum ChannelPrecision {
                CHANNEL_8_INT,
                CHANNEL_16_INT
            };

            Image(unsigned int, unsigned int, ImageFormat, std::vector<unsigned char>&&, bool);
            Image(unsigned int, unsigned int, ImageFormat, std::vector<uint16_t>&&, bool);
            ~Image() override = default;

            unsigned int getWidth() const;
            unsigned int getHeight() const;
            float getRatio() const;
            ImageFormat getImageFormat() const;
            unsigned int getComponentCount() const;
            ChannelPrecision getChannelPrecision() const;
            const std::vector<unsigned char>& getTexels() const;
            const std::vector<uint16_t>& getTexels16Bits() const;
            bool hasTransparency() const;

            unsigned int retrieveComponentsCount() const;
            TextureFormat retrieveTextureFormat() const;

        private:
            unsigned int width;
            unsigned int height;
            ImageFormat format;
            ChannelPrecision channelPrecision;
            std::vector<unsigned char> texels8; //8 bits
            std::vector<uint16_t> texels16; //16 bits
            bool bHasTransparency;
    };

}
