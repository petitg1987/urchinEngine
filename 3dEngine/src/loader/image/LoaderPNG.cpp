#include "UrchinCommon.h"

#include "LoaderPNG.h"

namespace urchin
{
    LoaderPNG::LoaderPNG() : Loader<Image>()
    {

    }

    Image *LoaderPNG::loadFromFile(const std::string &filename, void *params)
    {
        std::string filenamePath = FileSystem::instance()->getResourcesDirectory() + filename;
        std::vector<unsigned char> png;
        unsigned int errorLoad = lodepng::load_file(png, filenamePath);
        if(errorLoad!=0)
        {
            throw std::invalid_argument("Cannot load the file " + filenamePath + ": " + lodepng_error_text(errorLoad));
        }

        unsigned int width, height;
        lodepng::State state;
        state.info_raw.bitdepth=16;
        std::vector<unsigned char> image; //RGBA (16 bits)
        unsigned int errorRead = lodepng::decode(image, width, height, state, png);
        if(errorRead!=0)
        {
            throw std::invalid_argument("Cannot read the file " + filenamePath + ": " + lodepng_error_text(errorLoad));
        }

        unsigned int bitDepth = state.info_png.color.bitdepth;
        LodePNGColorType colorType = state.info_png.color.colortype;
        if(colorType==LodePNGColorType::LCT_GREY)
        {
            if(bitDepth==8)
            {

            }else if(bitDepth==16)
            {

            }else
            {
                throw std::invalid_argument("Unsupported number of bits for PNG image (grayscale): " + std::to_string(bitDepth));
            }
        }else if(colorType==LodePNGColorType::LCT_RGB)
        {
            if(bitDepth==8)
            {

            }else
            {
                throw std::invalid_argument("Unsupported number of bits for PNG image (RGB): " + std::to_string(bitDepth));
            }
        }else if(colorType==LodePNGColorType::LCT_RGBA)
        {
            if(bitDepth==8)
            {

            }else
            {
                throw std::invalid_argument("Unsupported number of bits for PNG image (RGBA): " + std::to_string(bitDepth));
            }
        }else
        {
            throw std::invalid_argument("Unsupported color type for PNG image: " + std::to_string(colorType));
        }

        long long r=0, g=0, b=0, a=0;
        for(unsigned int i=7; i<width*height*8; i+=8)
        {
            r += (static_cast<int>(image[i-7]) << 8) | static_cast<int>(image[i-6]); //TODO for 8bit: read one component (static_cast<int>(image[i-6]))
            g += (static_cast<int>(image[i-5]) << 8) | static_cast<int>(image[i-4]);
            b += (static_cast<int>(image[i-3]) << 8) | static_cast<int>(image[i-2]);
            a += (static_cast<int>(image[i-1]) << 8) | static_cast<int>(image[i-0]);
        }

        std::cout<<r<<" "<<g<<" "<<b<<" "<<a<<std::endl;

        //TODO devlop & test

        return new Image(width, height, Image::IMAGE_GRAYSCALE, &image[0]); //TODO build tab with new !
    }
}
