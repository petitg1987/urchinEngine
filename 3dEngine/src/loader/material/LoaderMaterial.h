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
            static constexpr char UV_SCALE_MESH_SCALE_X[] = "MESH_SCALE_X";
            static constexpr char UV_SCALE_MESH_SCALE_Y[] = "MESH_SCALE_Y";
            static constexpr char UV_SCALE_MESH_SCALE_Z[] = "MESH_SCALE_Z";
    };

}
