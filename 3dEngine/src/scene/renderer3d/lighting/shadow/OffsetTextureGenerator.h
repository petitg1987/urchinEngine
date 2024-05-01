#pragma once

namespace urchin {

    class OffsetTextureGenerator {
        public:
            OffsetTextureGenerator(unsigned int, unsigned int);

        private:
            std::vector<Vector2<float>> generateTextureData() const;
            void exportSVG(std::string, const std::vector<Vector2<float>>&) const;

            unsigned int textureXYSize;
            unsigned int filterXYSize;
    };

}