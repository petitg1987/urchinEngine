#include <stdexcept>

#include "TerrainMaterial.h"
#include "resources/MediaManager.h"

namespace urchin {

    TerrainMaterial::TerrainMaterial(const std::string& maskMapFilename, const std::vector<std::string>& materialFilenames, float sRepeat, float tRepeat) :
            maskMapFilename(maskMapFilename),
            sRepeat(sRepeat),
            tRepeat(tRepeat) {
        if (maskMapFilename.empty()) {
            maskTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 0, 0})).createTexture(false);
        } else {
            auto* maskImage = MediaManager::instance()->getMedia<Image>(maskMapFilename);
            if (maskImage->getImageFormat() != Image::IMAGE_RGBA) {
                maskImage->release();
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Components: " + std::to_string(maskImage->retrieveComponentsCount()));
            }
            maskTexture = maskImage->createTexture(false);
            maskImage->release();
        }

        initializeMaterial(materialFilenames);
    }

    TerrainMaterial::~TerrainMaterial() {
        for (auto& material : materials) {
            if (material != nullptr) {
                material->release();
            }
        }
    }

    void TerrainMaterial::initializeMaterial(const std::vector<std::string>& materialFilenames) {
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

    const std::string& TerrainMaterial::getMaskMapFilename() const {
        return maskMapFilename;
    }

    const std::shared_ptr<Texture>& TerrainMaterial::getMaskTexture() const {
        return maskTexture;
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

    const std::vector<Point2<float>>& TerrainMaterial::getTexCoordinates() const {
        return texCoordinates;
    }
}
