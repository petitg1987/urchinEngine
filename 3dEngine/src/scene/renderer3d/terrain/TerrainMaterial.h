#ifndef URCHINENGINE_TERRAINMATERIAL_H
#define URCHINENGINE_TERRAINMATERIAL_H

#include <string>
#include <vector>
#include "UrchinCommon.h"

#include "resources/material/Material.h"
#include "resources/image/Image.h"

#define MAX_MATERIAL 4 //maximum 4 materials (RGBA)

namespace urchin
{

    class TerrainMaterial
    {
        public:
            explicit TerrainMaterial(const std::string &, float, float);
            ~TerrainMaterial();

            void refreshWith(unsigned int, unsigned int);

            void addMaterial(unsigned int, const std::string &);

            const std::string &getMaskMapFilename() const;
            float getSRepeat() const;
            float getTRepeat() const;
            std::vector<Material *> getMaterials() const;
            const std::vector<Point2<float>> &getTexCoordinates() const;

            void loadTextures() const;

        private:
            std::vector<Point2<float>> buildTexCoordinates(unsigned int, unsigned int);

            std::string maskMapFilename;
            Image *maskTexture;
            float sRepeat;
            float tRepeat;
            std::vector<Material *> materials;
            std::vector<Point2<float>> texCoordinates;

            Image *defaultTexture;
    };

}

#endif
