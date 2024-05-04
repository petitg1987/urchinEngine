#pragma once

namespace urchin {

    class OffsetTextureGenerator {
        public:
            OffsetTextureGenerator(unsigned int, unsigned int);

            std::shared_ptr<Texture> getOffsetTexture() const;

        private:
            std::vector<std::vector<std::vector<Vector2<float>>>> generateTextureData() const;
            void exportSVG(std::string, const std::vector<std::vector<std::vector<Vector2<float>>>>&) const;

            unsigned int textureXYSize;
            unsigned int filterXYSize;

            std::shared_ptr<Texture> offsetTexture;
    };

}