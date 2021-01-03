#ifndef URCHINENGINE_LOADERPNG_H
#define URCHINENGINE_LOADERPNG_H

#include <string>

#include "resources/image/Image.h"
#include "loader/Loader.h"

namespace urchin {

    class LoaderPNG : public Loader<Image> {
        public:
            LoaderPNG();
            ~LoaderPNG() override = default;

            Image* loadFromFile(const std::string&, const std::map<std::string, std::string>&) override;

        private:
            static std::vector<unsigned char> extract8BitsChannels(const std::vector<unsigned char>&, unsigned int);
            static std::vector<uint16_t> extract16BitsChannels(const std::vector<unsigned char>&, unsigned int);
    };

}

#endif
