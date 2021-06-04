#pragma once

#include <string>

#include <resources/material/Material.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderMTR : public Loader<Material> {
        public:
            ~LoaderMTR() override = default;

            Material* loadFromFile(const std::string&, const std::map<std::string, std::string>&) override;
    };

}
