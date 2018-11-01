#include <GL/glew.h>
#include <stdexcept>

#include "TerrainMaterial.h"
#include "resources/MediaManager.h"

namespace urchin
{

    TerrainMaterial::TerrainMaterial(const std::string &maskMapFilename, float sRepeat, float tRepeat) :
            maskMapFilename(maskMapFilename),
            sRepeat(sRepeat),
            tRepeat(tRepeat)
    {
        if(maskMapFilename.empty())
        {
            maskTexture = new Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 0, 0}));
            maskTexture->toTexture(false, false, false);
        }else
        {
            maskTexture = MediaManager::instance()->getMedia<Image>(maskMapFilename);
            if(maskTexture->getImageFormat() != Image::IMAGE_RGBA)
            {
                maskTexture->release();
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Components: " + std::to_string(maskTexture->retrieveComponentsCount()));
            }
            maskTexture->toTexture(false, false, false);
        }

        materials.resize(MAX_MATERIAL);
        for (auto &material : materials)
        {
            material = nullptr;
        }

        defaultTexture = new Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 0, 0, 0}));
        defaultTexture->toTexture(false, false, false);
    }

    TerrainMaterial::~TerrainMaterial()
    {
        maskTexture->release();

        for (auto &material : materials)
        {
            if(material != nullptr)
            {
                material->release();
            }
        }

        delete defaultTexture;
    }

    void TerrainMaterial::refreshWith(unsigned int xSize, unsigned int zSize)
    {
        buildTexCoordinates(xSize, zSize);
    }

    void TerrainMaterial::addMaterial(unsigned int position, const std::string &materialFilename)
    {
        if(position >= materials.size())
        {
            throw std::runtime_error("Material position is incorrect. Value : " + std::to_string(position));
        }

        if(materials[position] != nullptr)
        {
            throw std::runtime_error("Material position " + std::to_string(position) + " is already used.");
        }

        materials[position] = MediaManager::instance()->getMedia<Material>(materialFilename);
    }

    const std::string &TerrainMaterial::getMaskMapFilename() const
    {
        return maskMapFilename;
    }

    float TerrainMaterial::getSRepeat() const
    {
        return sRepeat;
    }

    float TerrainMaterial::getTRepeat() const
    {
        return tRepeat;
    }

    std::vector<Material *> TerrainMaterial::getMaterials() const
    {
        return materials;
    }

    std::vector<Point2<float>> TerrainMaterial::buildTexCoordinates(unsigned int xSize, unsigned int zSize)
    {
        texCoordinates.reserve(xSize * zSize);

        for(unsigned int z = 0; z < zSize; ++z)
        {
            for (unsigned int x = 0; x < xSize; ++x)
            {
                float s = static_cast<float>(x) / xSize * sRepeat;
                float t = static_cast<float>(z) / zSize * tRepeat;

                texCoordinates.emplace_back(Point2<float>(s, t));
            }
        }

        return texCoordinates;
    }

    const std::vector<Point2<float>> &TerrainMaterial::getTexCoordinates() const
    {
        return texCoordinates;
    }

    void TerrainMaterial::loadTextures() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, maskTexture->getTextureID());

        for(unsigned int i=0; i<materials.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i + 1);

            if(materials[i] != nullptr)
            {
                glBindTexture(GL_TEXTURE_2D, materials[i]->getDiffuseTexture()->getTextureID());
            }else
            {
                glBindTexture(GL_TEXTURE_2D, defaultTexture->getTextureID());
            }
        }
    }
}
