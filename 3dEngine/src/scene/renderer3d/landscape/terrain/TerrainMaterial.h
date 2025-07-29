#pragma once

#include <string>
#include <vector>
#include <UrchinCommon.h>

#include "resources/material/Material.h"

namespace urchin {

    class TerrainMaterials {
        public:
            static constexpr unsigned int MAX_MATERIAL = 3;

            TerrainMaterials(std::string, const std::vector<std::string>&, float, float);

            void refreshWith(unsigned int, unsigned int);

            const std::string& getMaskMapFilename() const;
            const std::shared_ptr<Texture>& getMaskTexture() const;
            Vector2<float> getStRepeat() const;
            const std::array<std::shared_ptr<Material>, MAX_MATERIAL>& getMaterials() const;
            const std::vector<Point2<float>>& getTexCoordinates() const;

        private:
            void initializeMaterial(const std::vector<std::string>&);
            void buildTexCoordinates(unsigned int, unsigned int);
            Point2<float> computeTexCoordinates(unsigned int, unsigned int, unsigned int, unsigned int) const;

            std::string maskMapFilename;
            std::shared_ptr<Texture> maskTexture;
            float sRepeat;
            float tRepeat;
            std::array<std::shared_ptr<Material>, MAX_MATERIAL> materials;
            std::vector<Point2<float>> texCoordinates;
    };

}
