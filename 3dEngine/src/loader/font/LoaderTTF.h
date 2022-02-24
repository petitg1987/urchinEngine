#pragma once

#include <string>

#include <resources/font/Font.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderTTF : public Loader<Font> {
        public:
            ~LoaderTTF() override = default;

            std::shared_ptr<Font> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;
    };

}
