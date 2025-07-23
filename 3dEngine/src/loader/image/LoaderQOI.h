#pragma once

#include <string>

#include "resources/image/Image.h"
#include "loader/Loader.h"

namespace urchin {

    class LoaderQOI final : public Loader<Image> {
        public:
            ~LoaderQOI() override = default;

            std::shared_ptr<Image> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;
    };

}
