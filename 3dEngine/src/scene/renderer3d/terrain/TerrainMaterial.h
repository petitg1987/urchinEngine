#ifndef URCHINENGINE_TERRAINMATERIAL_H
#define URCHINENGINE_TERRAINMATERIAL_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "resources/material/Material.h"
#include "resources/image/Image.h"

namespace urchin
{

    class TerrainMaterial
    {
        public:
            explicit TerrainMaterial(const std::string &, float, float);
            ~TerrainMaterial();

            void addMaterial(unsigned int, const std::string &);

            const std::vector<Point2<float>> &getTexCoordinates() const;

            void initialize(unsigned int, unsigned int, unsigned int);
            void loadTextures() const;

            Image *getMaskTexture() const;

        private:
            std::vector<Point2<float>> buildTexCoordinates(unsigned int, unsigned int);

            bool isInitialized;

            Image *maskTexture;
            float sRepeat;
            float tRepeat;
            std::vector<Material *> materials;

            Image *defaultTexture;

            std::vector<Point2<float>> texCoordinates;
    };

}

#endif
