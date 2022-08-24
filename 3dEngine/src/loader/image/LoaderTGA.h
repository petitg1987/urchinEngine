#pragma once

#include <string>

#include <resources/image/Image.h>
#include <loader/Loader.h>

namespace urchin {

    #pragma pack(push, 1)
    struct TgaHeader {
        unsigned char idLength; //size of image id
        unsigned char colormapType; //1 is has a color map
        unsigned char imageType; //compression type

        short cmFirstEntry; //color map origin
        short cmLength; //color map length
        unsigned char cmSize; //color map size

        short xOrigin; //bottom left x coordinate origin
        short yOrigin; //bottom left y coordinate origin

        short width; //picture width (in pixels)
        short height; //picture height (in pixels)

        unsigned char pixelDepth; //bits per pixel: 8, 16, 24 or 32
        unsigned char imageDescriptor; //24 bits = 0x00; 32 bits = 0x80
    };
    #pragma pack(pop)

    class LoaderTGA final : public Loader<Image> {
        public:
            ~LoaderTGA() override = default;

            std::shared_ptr<Image> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;

        private:
            void getImageInfo(const TgaHeader&, unsigned int&, unsigned int&, unsigned int&, Image::ImageFormat&) const;
            void readTGA8bits(const std::vector<unsigned char>&, unsigned int, const std::vector<unsigned char>&, std::vector<unsigned char>&) const;
            void readTGA16bits(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGA24bits(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGA32bits(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGAGray8bits(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGA8bitsRLE(const std::vector<unsigned char>&, unsigned int, const std::vector<unsigned char>&, std::vector<unsigned char>&) const;
            void readTGA16bitsRLE(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGA24bitsRLE(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGA32bitsRLE(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
            void readTGAGray8bitsRLE(const std::vector<unsigned char>&, unsigned int, std::vector<unsigned char>&) const;
    };

}
