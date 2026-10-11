#pragma once

#include <memory>
#include <vector>

#include "scene/ui/Outline.h"
#include "resources/image/Image.h"
#include "graphics/api/GraphicsApi.h"

namespace urchin {

    class NineSlicingTexture {
        public:
            std::shared_ptr<Texture> build(const std::shared_ptr<Image>&, unsigned int, unsigned int, const Outline&) const;
            std::shared_ptr<Texture> build(const std::shared_ptr<Image>&, unsigned int, unsigned int, const Outline&, const Outline&) const;

        private:
            struct SourceTexelWeight {
                unsigned int index;
                float weight;
            };

            static std::vector<std::vector<SourceTexelWeight>> computeAxisMapping(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
            static void addScaledMapping(std::vector<SourceTexelWeight>&, unsigned int, unsigned int, unsigned int, unsigned int);
    };

}
