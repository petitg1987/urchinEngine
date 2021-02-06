#include <stdexcept>
#include <fstream>
#include "UrchinCommon.h"

#include "loader/image/LoaderTGA.h"

namespace urchin {

    LoaderTGA::LoaderTGA() : Loader<Image>(),
            colorMap(nullptr),
            data(nullptr),
            width(0),
            height(0),
            componentsCount(0),
            format(Image::IMAGE_RGBA) {

    }

    Image* LoaderTGA::loadFromFile(const std::string& filename, const std::map<std::string, std::string>&) {
        //opens file
        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
        std::ifstream file(filenamePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::invalid_argument("Unable to open file: " + filenamePath);
        }

        //gets file length
        file.seekg(0, std::ios::end);
        long length = (long)file.tellg();
        file.seekg(0, std::ios::beg);

        //extracts header
        TgaHeader header{};
        file.read((char*)&header, sizeof(TgaHeader));
        file.seekg(header.idLength, std::ios::cur);

        //extracts color map (color map is stored in BGR format)
        if (header.colormapType) {
            colorMap = new unsigned char[(std::size_t)(header.cmLength * (header.cmSize >> 3u))];
            file.read((char*)colorMap, header.cmLength * (header.cmSize >> 3u));
        }

        //memory allocation for rough pixel data
        long lengthData = length - (long)file.tellg();
        data = new unsigned char[(unsigned long)lengthData];
        file.read((char*)data, lengthData);

        //memory allocation for pixel data
        getImageInfo(header);
        texels.resize(width * height * componentsCount, 0);

        //reads image data
        switch(header.imageType) {
            case 1:
                //uncompressed 8 bits color index
                readTGA8bits();
                break;

            case 2:
                //uncompressed 16-24-32 bits
                switch(header.pixelDepth) {
                    case 16:
                        readTGA16bits();
                        break;

                    case 24:
                        readTGA24bits();
                        break;

                    case 32:
                        readTGA32bits();
                        break;
                    default:
                        break;
                }
                break;

            case 3:
                //uncompressed 8 bits grayscale
                if (header.pixelDepth == 8) {
                    readTGAGray8bits();
                } else {
                    throw std::runtime_error("Wrong number of bits for grayscale: " + std::to_string(header.pixelDepth));
                }
                break;

            case 9:
                //RLE compressed 8 bits color index
                readTGA8bitsRLE();
                break;

            case 10:
                //RLE compressed 16-24-32 bits
                switch(header.pixelDepth) {
                    case 16:
                        readTGA16bitsRLE();
                        break;

                    case 24:
                        readTGA24bitsRLE();
                        break;

                    case 32:
                        readTGA32bitsRLE();
                        break;
                    default:
                        break;
                }
                break;

            case 11:
                //RLE compressed 8bits grayscale
                if (header.pixelDepth == 8) {
                    readTGAGray8bitsRLE();
                } else {
                    throw std::runtime_error("Wrong number of bits for grayscale: " + std::to_string(header.pixelDepth));
                }
                break;

            default:
                if (header.colormapType) {
                    delete[] colorMap;
                }
                delete[] data;
                //image type is not correct
                throw std::runtime_error("Unknown TGA image type, filename: " + filenamePath + ".");
        }

        if (header.colormapType) {
            delete[] colorMap;
        }
        delete[] data;

        unsigned int origin = ((unsigned int)header.imageDescriptor & 0x20u) >> 5u; //0: origin bottom, 1: origin top
        bool addAlphaChannel = componentsCount == 3 && format == Image::IMAGE_RGBA;
        unsigned int expectedComponentsCount = addAlphaChannel ? 4 : componentsCount;

        std::vector<unsigned char> adjustedTexels(width * height * expectedComponentsCount, 0);
        for (unsigned int heightIndex = 0, heightInverseIndex = height - 1; heightIndex < height; heightIndex++, heightInverseIndex--) {
            unsigned int srcHeightIndex = (origin == 0) ? heightInverseIndex : heightIndex;
            for (unsigned int widthIndex = 0; widthIndex < width; widthIndex++) {

                for(unsigned int componentIndex = 0; componentIndex < componentsCount; ++componentIndex) {
                    unsigned int dstIndex = heightIndex * (width * expectedComponentsCount) + widthIndex * expectedComponentsCount + componentIndex;
                    unsigned int srcIndex = srcHeightIndex * (width * componentsCount) + widthIndex * componentsCount + componentIndex;
                    adjustedTexels[dstIndex] = texels[srcIndex];
                }

                if(addAlphaChannel) {
                    unsigned int dstAlphaIndex = heightIndex * (width * expectedComponentsCount) + widthIndex * expectedComponentsCount + 3;
                    adjustedTexels[dstAlphaIndex] = 255;
                }
            }
        }

        return new Image(width, height, format, std::move(adjustedTexels));
    }

    void LoaderTGA::getImageInfo(const TgaHeader& header) {
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

    void LoaderTGA::readTGA8bits() {
        unsigned char color;

        for (unsigned int i = 0; i < width * height; ++i) {
            //reads index color byte
            color = data[i];

            //converts to RGB 24 bits
            texels[(i * 3) + 2] = colorMap[(color * 3) + 0];
            texels[(i * 3) + 1] = colorMap[(color * 3) + 1];
            texels[(i * 3) + 0] = colorMap[(color * 3) + 2];
        }
    }

    void LoaderTGA::readTGA16bits() {
        unsigned short color;

        for (unsigned int i = 0, j = 0;i < width * height; ++i, j += 2) {
            //reads color word
            color = (unsigned short)(data[j] + (data[j + 1] << 8u));

            //converts BGR to RGB
            texels[(i * 3) + 0] = static_cast<unsigned char>(((color & 0x7C00u) >> 10u) << 3u);
            texels[(i * 3) + 1] = static_cast<unsigned char>(((color & 0x03E0u) >>  5u) << 3u);
            texels[(i * 3) + 2] = static_cast<unsigned char>(((color & 0x001Fu) >>  0u) << 3u);
        }
    }

    void LoaderTGA::readTGA24bits() {
        for (unsigned int i = 0, j = 0; i < width * height; ++i, j += 3) {
            //reads and converts BGR to RGB
            texels[(i * 3) + 2] = data[j + 0];
            texels[(i * 3) + 1] = data[j + 1];
            texels[(i * 3) + 0] = data[j + 2];
        }
    }

    void LoaderTGA::readTGA32bits() {
        for (unsigned int i = 0, j = 0; i < width * height; ++i, j += 4) {
            //reads and converts BGRA to RGBA
            texels[(i * 4) + 2] = data[j + 0];
            texels[(i * 4) + 1] = data[j + 1];
            texels[(i * 4) + 0] = data[j + 2];
            texels[(i * 4) + 3] = data[j + 3];
        }
    }

    void LoaderTGA::readTGAGray8bits() {
        for (unsigned int i = 0; i < width * height; ++i) {
            texels[i] = data[i];
        }
    }

    void LoaderTGA::readTGA8bitsRLE() {
        int j = 0;
        unsigned char color;
        unsigned char packetHeader;
        unsigned int ptrIndex = 0;

        while (ptrIndex < width * height * 3) {
            //reads first byte
            packetHeader = data[j++];
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

    void LoaderTGA::readTGA16bitsRLE() {
        int j = 0;
        unsigned short color;
        unsigned char packetHeader;
        unsigned int ptrIndex = 0;

        while (ptrIndex < width * height * 3) {
            //reads first byte
            packetHeader = data[j++];
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

    void LoaderTGA::readTGA24bitsRLE() {
        int j = 0;
        unsigned char *rgb;
        unsigned char packetHeader;
        unsigned int ptrIndex = 0;

        while (ptrIndex < width * height * 3) {
            //reads first byte
            packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                rgb = &data[j];
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

    void LoaderTGA::readTGA32bitsRLE() {
        int j = 0;
        unsigned char *rgba;
        unsigned char packetHeader;
        unsigned int ptrIndex = 0;

        while (ptrIndex < width * height * 4) {
            //reads first byte
            packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                rgba = &data[j];
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

    void LoaderTGA::readTGAGray8bitsRLE() {
        unsigned int j = 0;
        unsigned char color;
        unsigned char packetHeader;
        unsigned int ptrIndex = 0;

        while (ptrIndex < width*height) {
            //reads first byte
            packetHeader = data[j++];
            unsigned int size = 1 + (packetHeader & 0x7fu);

            if (packetHeader & 0x80u) {
                //run-length packet
                color = data[j++];

                for (unsigned int i = 0; i < size; ++i, ptrIndex++) {
                    texels[ptrIndex] = color;
                }
            } else {
                //non run-length packet
                for (unsigned int i = 0; i < size; ++i) {
                    texels[ptrIndex+i] = data[j+i];
                }

                ptrIndex += size;
                j += size;
            }
        }
    }

}
