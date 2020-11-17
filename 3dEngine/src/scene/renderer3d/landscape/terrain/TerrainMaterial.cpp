#include <stdexcept>

#include "TerrainMaterial.h"
#include "resources/MediaManager.h"

namespace urchin {

    TerrainMaterial::TerrainMaterial(const std::string &maskMapFilename, const std::vector<std::string> &materialFilenames, float sRepeat, float tRepeat) :
            maskMapFilename(maskMapFilename),
            sRepeat(sRepeat),
            tRepeat(tRepeat) {
        if (maskMapFilename.empty()) {
            maskTexture = new Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 0, 0}));
            maskTexture->toTexture(false);
        } else {
            maskTexture = MediaManager::instance()->getMedia<Image>(maskMapFilename);
            if (maskTexture->getImageFormat() != Image::IMAGE_RGBA) {
                maskTexture->release();
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Components: " + std::to_string(maskTexture->retrieveComponentsCount()));
            }
            maskTexture->toTexture(false);
        }

        defaultTexture = new Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({0, 0, 0, 0}));
        defaultTexture->toTexture(false);

        initializeMaterial(materialFilenames);
    }

    TerrainMaterial::~TerrainMaterial() {
        maskTexture->release();

        for (auto &material : materials) {
            if (material != nullptr) {
                material->release();
            }
        }

        delete defaultTexture;
    }

    void TerrainMaterial::initializeMaterial(const std::vector<std::string> &materialFilenames) {
        assert(materialFilenames.size() <= MAX_MATERIAL);

        materials.resize(MAX_MATERIAL);
        for(std::size_t i = 0; i < MAX_MATERIAL; ++i) {
            if (materialFilenames.size() > i && !materialFilenames[i].empty()) {
                materials[i] = MediaManager::instance()->getMedia<Material>(materialFilenames[i]);
            } else {
                materials[i] = nullptr;
            }
        }
    }

    void TerrainMaterial::refreshWith(unsigned int xSize, unsigned int zSize) {
        buildTexCoordinates(xSize, zSize);
    }

    const std::string &TerrainMaterial::getMaskMapFilename() const {
        return maskMapFilename;
    }

    unsigned int TerrainMaterial::getMaskTexture() const {
        return maskTexture->getTextureID();
    }

    float TerrainMaterial::getSRepeat() const {
        return sRepeat;
    }

    float TerrainMaterial::getTRepeat() const {
        return tRepeat;
    }

    std::vector<Material *> TerrainMaterial::getMaterials() const {
        return materials;
    }

    void TerrainMaterial::buildTexCoordinates(unsigned int xSize, unsigned int zSize) {
        texCoordinates.reserve(xSize * zSize);

        for (unsigned int z = 0; z < zSize; ++z) {
            for (unsigned int x = 0; x < xSize; ++x) {
                float s = (float)x / (float)xSize * sRepeat;
                float t = (float)z / (float)zSize * tRepeat;

                texCoordinates.emplace_back(Point2<float>(s, t));
            }
        }
    }

    const std::vector<Point2<float>> &TerrainMaterial::getTexCoordinates() const {
        return texCoordinates;
    }
}
