#pragma once

#include <string>

#include <resources/image/Image.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderPNG : public Loader<Image> {
        public:
            ~LoaderPNG() override = default;

            std::shared_ptr<Image> loadFromFile(const std::string&, const std::map<std::string, std::string>&) override;

        private:
            std::vector<unsigned char> extract8BitsChannels(const std::vector<unsigned char>&, unsigned int, bool) const;
            std::vector<uint16_t> extract16BitsChannels(const std::vector<unsigned char>&, unsigned int) const;
    };

}
