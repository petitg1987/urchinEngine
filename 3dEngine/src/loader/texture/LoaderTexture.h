#pragma once

#include <string>

#include "graphics/api/GraphicsApi.h"
#include "loader/Loader.h"

namespace urchin {

    class LoaderTexture final : public Loader<Texture> {
        public:
            static const std::string DEFAULT_TEXTURE_FILENAME;

            ~LoaderTexture() override = default;

            std::shared_ptr<Texture> loadFromFile(const std::string&, const std::map<std::string, std::string, std::less<>>&) override;
    };

}
