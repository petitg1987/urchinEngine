#include <stdexcept>
#include <fstream>
#include <UrchinCommon.h>

#include <loader/image/LoaderTGA.h>

namespace urchin {

    std::shared_ptr<Image> LoaderTGA::loadFromFile(const std::string& filename, const std::map<std::string, std::string, std::less<>>&) {
        //open file
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filename);
        }

        //get file length
        file.seekg(0, std::ios::end);
        auto length = (long)file.tellg();
        file.seekg(0, std::ios::beg);

        //extract header
        TgaHeader header{};
        file.read((char*)&header, sizeof(TgaHeader));
        file.seekg(header.idLength, std::ios::cur);

        //extract color map (color map is stored in BGR format)
        std::vector<unsigned char> colorMap;
        if (header.colormapType) {
            colorMap.resize((std::size_t)header.cmLength * (std::size_t)(header.cmSize >> 3u), 0);
            file.read((char*)colorMap.data(), header.cmLength * (header.cmSize >> 3u));
        }

        //memory allocation for rough pixel data
        std::vector<unsigned char> data;
        long lengthData = length - (long)file.tellg();
        data.resize((std::size_t)lengthData);
        file.read((char*)data.data(), lengthData);

        //memory allocation for pixel data
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int componentsCount = 0;
        Image::ImageFormat format = Image::IMAGE_RGBA;
        getImageInfo(header, width, height, componentsCount, format);
        std::vector<unsigned char> texels;
        texels.resize(width * height * componentsCount, 0);

        //read image data
        unsigned int imageSize = width * height;
        switch(header.imageType) {
            case 1:
                //uncompressed 8 bits color index
                readTGA8bits(data, imageSize, colorMap, texels);
                break;

            case 2:
                //uncompressed 16-24-32 bits
                switch(header.pixelDepth) {
                    case 16:
                        readTGA16bits(data, imageSize, texels);
                        break;

                    case 24:
                        readTGA24bits(data, imageSize, texels);
                        break;

                    case 32:
                        readTGA32bits(data, imageSize, texels);
                        break;
                    default:
                        break;
                }
                break;

            case 3:
                //uncompressed 8 bits grayscale
                if (header.pixelDepth == 8) {
                    readTGAGray8bits(data, imageSize, texels);
                } else {
                    throw std::runtime_error("Wrong number of bits for grayscale: " + std::to_string(header.pixelDepth));
                }
                break;

            case 9:
                //RLE compressed 8 bits color index
                readTGA8bitsRLE(data, imageSize, colorMap, texels);
                break;

            case 10:
                //RLE compressed 16-24-32 bits
                switch(header.pixelDepth) {
                    case 16:
                        readTGA16bitsRLE(data, imageSize, texels);
                        break;

                    case 24:
                        readTGA24bitsRLE(data, imageSize, texels);
                        break;

                    case 32:
                        readTGA32bitsRLE(data, imageSize, texels);
                        break;
                    default:
                        break;
                }
                break;

            case 11:
                //RLE compressed 8bits grayscale
                if (header.pixelDepth == 8) {
                    readTGAGray8bitsRLE(data, imageSize, texels);
                } else {
                    throw std::runtime_error("Wrong number of bits for grayscale: " + std::to_string(header.pixelDepth));
                }
                break;

            default:
                //image type is not correct
                throw std::runtime_error("Unknown TGA image type, filename: " + filename + ".");
        }

        unsigned int origin = ((unsigned int)header.imageDescriptor & 0x20u) >> 5u; //0: origin bottom, 1: origin top
        bool addAlphaChannel = componentsCount == 3 && format == Image::IMAGE_RGBA;
        unsigned int expectedComponentsCount = addAlphaChannel ? 4 : componentsCount;
        bool hasTransparency = false;

        std::vector<unsigned char> adjustedTexels(width * height * expectedComponentsCount, 255 /* 255 for alpha channel */);
        for (unsigned int heightIndex = 0, heightInverseIndex = height - 1; heightIndex < height; heightIndex++, heightInverseIndex--) {
            unsigned int srcHeightIndex = (origin == 0) ? heightInverseIndex : heightIndex;

            for (unsigned int widthIndex = 0; widthIndex < width; widthIndex++) {
                for (unsigned int componentIndex = 0; componentIndex < componentsCount; ++componentIndex) {
                    unsigned int dstIndex = heightIndex * (width * expectedComponentsCount) + widthIndex * expectedComponentsCount + componentIndex;
                    unsigned int srcIndex = srcHeightIndex * (width * componentsCount) + widthIndex * componentsCount + componentIndex;
                    adjustedTexels[dstIndex] = texels[srcIndex];

                    if (!hasTransparency && componentIndex == 3 && adjustedTexels[dstIndex] < 255) {
                        hasTransparency = true;
                    }
                }
            }
        }

        return std::make_shared<Image>(width, height, format, std::move(adjustedTexels), hasTransparency);
    }

    /**
     * @param width [out] Image width
     * @param height [out] Image height
     * @param componentsCount [out] Number of components
     * @param format [out] Image format
     */
    void LoaderTGA::getImageInfo(const TgaHeader& header, unsigned int& width, unsigned int& height, unsigned int& componentsCount, Image::ImageFormat& format) const {
        width = (unsigned int)header.width;
        height = (unsigned int)header.height;

        switch(header.imageType) {
            case 3: //grayscale 8 bits
            case 11: { //grayscale 8 bits (RLE)
                if (header.pixelDepth == 8) {
                    format = Image::IMAGE_GRAYSCALE;
                    componentsCount = 1;
                } else {
                    throw std::runtime_error("Wrong number of bits for grayscale: " + std::to_string(header.pixelDepth));
                }
                break;
            }
            case 1: //8 bits color index
            case 2: //BGR 16-24-32 bits
            case 9: //8 bits color index (RLE)
            case 10: { //BGR 16-24-32 bits (RLE)
                format = Image::IMAGE_RGBA;
                //8 bits and 16 bits images will be converted to 24 bits
                if (header.pixelDepth <= 24) {
                    componentsCount = 3;
                } else { //32 bits
                    componentsCount = 4;
                }
                break;
            }
            default:
                break;
        }
    }

    void LoaderTGA::readTGA8bits(const std::vector<unsigned char>& data, unsigned int imageSize, const std::vector<unsigned char>& colorMap, std::vector<unsigned char>& texels) const {
        for (unsigned int i = 0; i < imageSize; ++i) {
            //reads index color byte
            unsigned char color = data[i];

            //converts to RGB 24 bits
            texels[(i * 3) + 2] = colorMap[(color * 3) + 0];
            texels[(i * 3) + 1] = colorMap[(color * 3) + 1];
            texels[(i * 3) + 0] = colorMap[(color * 3) + 2];
        }
    }

    void LoaderTGA::readTGA16bits(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        for (unsigned int i = 0, j = 0;i < imageSize; ++i, j += 2) {
            //reads color word
            auto color = (unsigned short)(data[j] + (data[j + 1] << 8u));

            //converts BGR to RGB
            texels[(i * 3) + 0] = static_cast<unsigned char>(((color & 0x7C00u) >> 10u) << 3u);
            texels[(i * 3) + 1] = static_cast<unsigned char>(((color & 0x03E0u) >>  5u) << 3u);
            texels[(i * 3) + 2] = static_cast<unsigned char>(((color & 0x001Fu) >>  0u) << 3u);
        }
    }

    void LoaderTGA::readTGA24bits(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        for (unsigned int i = 0, j = 0; i < imageSize; ++i, j += 3) {
            //reads and converts BGR to RGB
            texels[(i * 3) + 2] = data[j + 0];
            texels[(i * 3) + 1] = data[j + 1];
            texels[(i * 3) + 0] = data[j + 2];
        }
    }

    void LoaderTGA::readTGA32bits(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        for (unsigned int i = 0, j = 0; i < imageSize; ++i, j += 4) {
            //reads and converts BGRA to RGBA
            texels[(i * 4) + 2] = data[j + 0];
            texels[(i * 4) + 1] = data[j + 1];
            texels[(i * 4) + 0] = data[j + 2];
            texels[(i * 4) + 3] = data[j + 3];
        }
    }

    void LoaderTGA::readTGAGray8bits(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        for (unsigned int i = 0; i < imageSize; ++i) {
            texels[i] = data[i];
        }
    }

    void LoaderTGA::readTGA8bitsRLE(const std::vector<unsigned char>& data, unsigned int imageSize, const std::vector<unsigned char>& colorMap, std::vector<unsigned char>& texels) const {
        std::size_t j = 0;
        unsigned char color;
        unsigned int ptrIndex = 0;

        while (ptrIndex < imageSize * 3) {
            //reads first byte
            unsigned char  packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                color = data[j++];

                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3) {
                    texels[ptrIndex] = colorMap[(color * 3) + 2];
                    texels[ptrIndex + 1] = colorMap[(color * 3) + 1];
                    texels[ptrIndex + 2] = colorMap[(color * 3) + 0];
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3) {
                    color = data[j++];

                    texels[ptrIndex] = colorMap[(color * 3) + 2];
                    texels[ptrIndex + 1] = colorMap[(color * 3) + 1];
                    texels[ptrIndex + 2] = colorMap[(color * 3) + 0];
                }
            }
        }
    }

    void LoaderTGA::readTGA16bitsRLE(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        std::size_t j = 0;
        unsigned short color;
        unsigned int ptrIndex = 0;

        while (ptrIndex < imageSize * 3) {
            //reads first byte
            unsigned char packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                color = (unsigned short)(data[j] + (data[j + 1] << 8u));
                j += 2;

                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3) {
                    texels[ptrIndex] = static_cast<unsigned char>(((color & 0x7C00u) >> 10u) << 3u);
                    texels[ptrIndex + 1] = static_cast<unsigned char>(((color & 0x03E0u) >>  5u) << 3u);
                    texels[ptrIndex + 2] = static_cast<unsigned char>(((color & 0x001Fu) >>  0u) << 3u);
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3, j += 2) {
                    color = (unsigned short)(data[j] + (data[j + 1] << 8u));

                    texels[ptrIndex] = static_cast<unsigned char>(((color & 0x7C00u) >> 10u) << 3u);
                    texels[ptrIndex + 1] = static_cast<unsigned char>(((color & 0x03E0u) >> 5u) << 3u);
                    texels[ptrIndex + 2] = static_cast<unsigned char>(((color & 0x001Fu) >> 0u) << 3u);
                }
            }
        }
    }

    void LoaderTGA::readTGA24bitsRLE(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        std::size_t j = 0;
        unsigned int ptrIndex = 0;

        while (ptrIndex < imageSize * 3) {
            //reads first byte
            unsigned char packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                const unsigned char *rgb = &data[j];
                j += 3;

                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3) {
                    texels[ptrIndex] = rgb[2];
                    texels[ptrIndex + 1] = rgb[1];
                    texels[ptrIndex + 2] = rgb[0];
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0; i < size; ++i, ptrIndex += 3, j += 3) {
                    texels[ptrIndex + 2] = data[j + 0];
                    texels[ptrIndex + 1] = data[j + 1];
                    texels[ptrIndex] = data[j + 2];
                }
            }
        }
    }

    void LoaderTGA::readTGA32bitsRLE(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        std::size_t j = 0;
        unsigned int ptrIndex = 0;

        while (ptrIndex < imageSize * 4) {
            //reads first byte
            unsigned char packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                const unsigned char *rgba = &data[j];
                j += 4;

                for (unsigned int i = 0; i < size; ++i, ptrIndex += 4) {
                    texels[ptrIndex] = rgba[2];
                    texels[ptrIndex + 1] = rgba[1];
                    texels[ptrIndex + 2] = rgba[0];
                    texels[ptrIndex + 3] = rgba[3];
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0 ; i < size; ++i, ptrIndex += 4, j += 4) {
                    texels[ptrIndex + 2] = data[j + 0];
                    texels[ptrIndex + 1] = data[j + 1];
                    texels[ptrIndex] = data[j + 2];
                    texels[ptrIndex + 3] = data[j + 3];
                }
            }
        }
    }

    void LoaderTGA::readTGAGray8bitsRLE(const std::vector<unsigned char>& data, unsigned int imageSize, std::vector<unsigned char>& texels) const {
        unsigned int j = 0;
        unsigned int ptrIndex = 0;

        while (ptrIndex < imageSize) {
            //reads first byte
            unsigned char packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                unsigned char color = data[j++];

                for (unsigned int i = 0; i < size; ++i, ptrIndex++) {
                    texels[ptrIndex] = color;
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0; i < size; ++i) {
                    texels[ptrIndex + i] = data[j + i];
                }

                ptrIndex += size;
                j += size;
            }
        }
    }

}
