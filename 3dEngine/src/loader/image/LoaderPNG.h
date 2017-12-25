#ifndef URCHINENGINE_LOADERPNG_H
#define URCHINENGINE_LOADERPNG_H

#include <string>

#include "resources/image/Image.h"
#include "loader/Loader.h"

namespace urchin
{

    class LoaderPNG : public Loader<Image>
    {
        public:
            LoaderPNG();
            ~LoaderPNG() override = default;

            Image *loadFromFile(const std::string &, void *) override;
    };

}

#endif
