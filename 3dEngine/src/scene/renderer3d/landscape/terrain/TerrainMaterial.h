#ifndef URCHINENGINE_TERRAINMATERIAL_H
#define URCHINENGINE_TERRAINMATERIAL_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "resources/material/Material.h"
#include "resources/image/Image.h"

namespace urchin {

    class TerrainMaterial {
        public:
            static const unsigned int MAX_MATERIAL;

            explicit TerrainMaterial(const std::string&, const std::vector<std::string>&, float, float);
            ~TerrainMaterial();

            void refreshWith(unsigned int, unsigned int);

            const std::string& getMaskMapFilename() const;
            const std::shared_ptr<Texture>& getMaskTexture() const;
            float getSRepeat() const;
            float getTRepeat() const;
            std::vector<Material*> getMaterials() const;
            const std::vector<Point2<float>>& getTexCoordinates() const;

        private:
            void initializeMaterial(const std::vector<std::string>&);
            void buildTexCoordinates(unsigned int, unsigned int);

            std::string maskMapFilename;
            std::shared_ptr<Texture> maskTexture;
            float sRepeat;
            float tRepeat;
            std::vector<Material*> materials;
            std::vector<Point2<float>> texCoordinates;
    };

}

#endif
