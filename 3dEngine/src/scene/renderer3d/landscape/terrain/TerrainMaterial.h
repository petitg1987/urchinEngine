#pragma once

#include <string>
#include <vector>
#include <UrchinCommon.h>

#include <resources/material/Material.h>
#include <resources/image/Image.h>

namespace urchin {

    class TerrainMaterials {
        public:
            static const unsigned int MAX_MATERIAL;

            explicit TerrainMaterials(const std::string&, const std::vector<std::string>&, float, float);

            void refreshWith(unsigned int, unsigned int);

            const std::string& getMaskMapFilename() const;
            const std::shared_ptr<Texture>& getMaskTexture() const;
            Vector2<float> getStRepeat() const;
            const std::vector<std::shared_ptr<Material>>& getMaterials() const;
            const std::vector<Point2<float>>& getTexCoordinates() const;

        private:
            void initializeMaterial(const std::vector<std::string>&);
            void buildTexCoordinates(unsigned int, unsigned int);

            std::string maskMapFilename;
            std::shared_ptr<Texture> maskTexture;
            float sRepeat;
            float tRepeat;
            std::vector<std::shared_ptr<Material>> materials;
            std::vector<Point2<float>> texCoordinates;
    };

}
