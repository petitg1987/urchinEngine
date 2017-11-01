#include <GL/glew.h>
#include <stdexcept>

#include "TerrainMaterial.h"
#include "resources/MediaManager.h"

namespace urchin
{

    TerrainMaterial::TerrainMaterial(const std::string &maskMapFilename) :
            maskTexture(MediaManager::instance()->getMedia<Image>(maskMapFilename, nullptr))
    {
        if(maskTexture->getComponentsCount() != 4)
        {
            throw std::runtime_error("Mask texture must have 4 component (RGBA). Components count: " + std::to_string(maskTexture->getComponentsCount()));
        }
        maskTexture->toTexture(false, false, false);

        for(unsigned int i=0; i<4; ++i)
        {
            materials[i] = nullptr;
        }

        auto *defaultDiffuseColorTab = new unsigned char[4]{0, 0, 0, 0};
        defaultTexture = new Image(4, 1, 1, Image::IMAGE_RGBA, defaultDiffuseColorTab);
        defaultTexture->toTexture(false, false, false);
    }

    TerrainMaterial::~TerrainMaterial()
    {
        for(unsigned int i=0; i<4; ++i)
        {
            if(materials[i] != nullptr)
            {
                materials[i]->release();
            }
        }

        delete defaultTexture;
    }

    void TerrainMaterial::addMaterial(unsigned int position, const std::string &materialFilename)
    {
        if(position >= 4)
        {
            throw std::runtime_error("Material position must be between 0 and 3. Value : " + std::to_string(position));
        }

        if(materials[position] != nullptr)
        {
            throw std::runtime_error("Material position " + std::to_string(position) + " is already used.");
        }

        materials[position] = MediaManager::instance()->getMedia<Material>(materialFilename, nullptr);
    }

    void TerrainMaterial::initialize(unsigned int shader)
    {
        int maskTexLoc = glGetUniformLocation(shader, "maskTex");
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(maskTexLoc, 0);

        for(unsigned int i=0; i<4; ++i)
        {
            std::string shaderTextureName = "diffuseTex" + std::to_string(i + 1);
            int diffuseTexLoc = glGetUniformLocation(shader, shaderTextureName.c_str());

            glActiveTexture(GL_TEXTURE0 + i + 1);
            glUniform1i(diffuseTexLoc, i + 1);
        }
    }

    void TerrainMaterial::loadTextures() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, maskTexture->getTextureID());

        for(unsigned int i=0; i<4; ++i)
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
