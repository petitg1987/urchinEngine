#ifndef URCHINENGINE_TERRAINMATERIAL_H
#define URCHINENGINE_TERRAINMATERIAL_H

#include <string>

#include "resources/material/Material.h"
#include "resources/image/Image.h"

namespace urchin
{

    class TerrainMaterial
    {
        public:
            explicit TerrainMaterial(const std::string &);
            ~TerrainMaterial();

            void addMaterial(unsigned int, const std::string &);

            void initialize(unsigned int);
            void loadTextures() const;

            Image *getMaskTexture() const;

        private:
            Image *maskTexture;
            Material *materials[4]; //TODO replace by vector
            Image *defaultTexture;
    };

}

#endif
