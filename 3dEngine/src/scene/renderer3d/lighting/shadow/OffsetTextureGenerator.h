#pragma once

#include <vector>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class OffsetTextureGenerator {
        public:
            OffsetTextureGenerator(unsigned int, unsigned int);

            std::shared_ptr<Texture> getOffsetTexture() const;

        private:
            std::vector<Vector2<float>> generateTextureData() const;
            void exportSVG(std::string, const std::vector<Vector2<float>>&) const;

            unsigned int textureXYSize;
            unsigned int filterBoxSize;

            std::shared_ptr<Texture> offsetTexture;
    };

}