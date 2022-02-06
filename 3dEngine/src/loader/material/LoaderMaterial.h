#pragma once

#include <string>

#include <resources/material/Material.h>
#include <loader/Loader.h>

namespace urchin {

    class LoaderMaterial : public Loader<Material> {
        public:
            ~LoaderMaterial() override = default;

            std::shared_ptr<Material> loadFromFile(const std::string&, const std::map<std::string, std::string>&) override;

        private:
            UvScaleType toUvScaleType(const std::string&, const std::string&) const;

            static constexpr char UV_SCALE_NONE[] = "NONE";
            static constexpr char SCALE_ON_AXIS_ALIGNED_FACES[] = "SCALE_ON_AXIS_ALIGNED_FACES";
            static constexpr char SCALE_ON_XY_FACES[] = "SCALE_ON_XY_FACES";
            static constexpr char SCALE_ON_XZ_FACES[] = "SCALE_ON_XZ_FACES";
            static constexpr char SCALE_ON_YZ_FACES[] = "SCALE_ON_YZ_FACES";
    };

}
