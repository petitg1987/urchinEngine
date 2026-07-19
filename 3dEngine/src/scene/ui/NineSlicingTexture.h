#pragma once

#include <memory>

#include "scene/ui/Outline.h"
#include "resources/image/Image.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class NineSlicingTexture {
        public:
            std::shared_ptr<Texture> build(const std::shared_ptr<Image>&, unsigned int, unsigned int, const Outline&) const;
    };

}
