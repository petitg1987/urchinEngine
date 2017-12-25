#include "UrchinCommon.h"

#include "LoaderPNG.h"

namespace urchin
{
    LoaderPNG::LoaderPNG() : Loader<Image>()
    {

    }

    Image *LoaderPNG::loadFromFile(const std::string &filename, void *params)
    {
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels
        unsigned width, height;
        lodepng::State state; //optionally customize this one

        unsigned int errorLoad = lodepng::load_file(png, filename);
        if(errorLoad!=0)
        {
            throw std::invalid_argument("Cannot load the file " + filename + ": " + lodepng_error_text(errorLoad));
        }

        unsigned int errorRead = lodepng::decode(image, width, height, state, png);
        if(errorRead!=0)
        {
            throw std::invalid_argument("Cannot read the file " + filename + ": " + lodepng_error_text(errorLoad));
        }

        //TODO devlop & test

        return new Image(1, width, height, Image::IMAGE_GRAYSCALE_16BITS, &image[0]);
    }
}
