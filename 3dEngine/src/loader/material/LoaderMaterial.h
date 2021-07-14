#pragma once

#include <string>

#include <resources/material/Material.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderMaterial : public Loader<Material> {
        public:
            ~LoaderMaterial() override = default;

            std::shared_ptr<Material> loadFromFile(const std::string&, const std::map<std::string, std::string>&) override;
    };

}
