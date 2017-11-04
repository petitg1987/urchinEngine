#include <GL/glew.h>
#include <stdexcept>

#include "TerrainMaterial.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"

namespace urchin
{

    TerrainMaterial::TerrainMaterial(const std::string &maskMapFilename, float sRepeat, float tRepeat) :
            isInitialized(false),
            sRepeat(sRepeat),
            tRepeat(tRepeat),
            maskTexture(MediaManager::instance()->getMedia<Image>(maskMapFilename, nullptr))
    {
        if(maskTexture->getComponentsCount() != 4)
        {
            throw std::runtime_error("Mask texture must have 4 component (RGBA). Components count: " + std::to_string(maskTexture->getComponentsCount()));
        }
        maskTexture->toTexture(false, false, false);

        materials.resize(4); //maximum 4 materials (RGBA)
        for (auto &material : materials)
        {
            material = nullptr;
        }

        auto *defaultDiffuseColorTab = new unsigned char[4]{0, 0, 0, 0};
        defaultTexture = new Image(4, 1, 1, Image::IMAGE_RGBA, defaultDiffuseColorTab);
        defaultTexture->toTexture(false, false, false);
    }

    TerrainMaterial::~TerrainMaterial()
    {
        for (auto &material : materials)
        {
            if(material != nullptr)
            {
                material->release();
            }
        }

        delete defaultTexture;
    }

    void TerrainMaterial::addMaterial(unsigned int position, const std::string &materialFilename)
    {
        if(isInitialized)
        {
            throw std::runtime_error("Impossible to add material once the terrain displayer initialized.");
        }

        if(position >= materials.size())
        {
            throw std::runtime_error("Material position is incorrect. Value : " + std::to_string(position));
        }

        if(materials[position] != nullptr)
        {
            throw std::runtime_error("Material position " + std::to_string(position) + " is already used.");
        }

        materials[position] = MediaManager::instance()->getMedia<Material>(materialFilename, nullptr);
    }

    void TerrainMaterial::initialize(unsigned int shader, unsigned int xSize, unsigned int zSize)
    {
        if(isInitialized)
        {
            throw std::runtime_error("Terrain displayer is already initialized.");
        }

        ShaderManager::instance()->bind(shader);
        int maskTexLoc = glGetUniformLocation(shader, "maskTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(maskTexLoc, 0);
        for(unsigned int i=0; i<materials.size(); ++i)
        {
            std::string shaderTextureName = "diffuseTex" + std::to_string(i + 1);
            int diffuseTexLoc = glGetUniformLocation(shader, shaderTextureName.c_str());

            glActiveTexture(GL_TEXTURE0 + i + 1);
            glUniform1i(diffuseTexLoc, i + 1);
        }
        int sRepeatLoc = glGetUniformLocation(shader, "sRepeat");
        glUniform1f(sRepeatLoc, sRepeat);
        int tRepeatLoc = glGetUniformLocation(shader, "tRepeat");
        glUniform1f(tRepeatLoc, tRepeat);

        buildTexCoordinates(xSize, zSize);

        isInitialized = true;
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
        if(!isInitialized)
        {
            throw std::runtime_error("Terrain displayer must be initialized before load textures.");
        }

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

    Image *TerrainMaterial::getMaskTexture() const
    {
        return maskTexture;
    }
}
