#include <stdexcept>

#include <scene/renderer3d/landscape/terrain/TerrainMaterial.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    TerrainMaterials::TerrainMaterials(const std::string& maskMapFilename, const std::vector<std::string>& materialFilenames, float sRepeat, float tRepeat) :
            maskMapFilename(maskMapFilename),
            sRepeat(sRepeat),
            tRepeat(tRepeat) {
        if (maskMapFilename.empty()) {
            maskTexture = Image(1, 1, Image::IMAGE_RGBA, std::vector<unsigned char>({255, 0, 0, 0}), true).createTexture(false);
        } else {
            auto maskImage = ResourceRetriever::instance().getResource<Image>(maskMapFilename);
            if (maskImage->getImageFormat() != Image::IMAGE_RGBA) {
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Components: " + std::to_string(maskImage->retrieveComponentsCount()));
            }
            maskTexture = maskImage->createTexture(false);
        }

        initializeMaterial(materialFilenames);
    }

    void TerrainMaterials::initializeMaterial(const std::vector<std::string>& materialFilenames) {
        assert(materialFilenames.size() <= MAX_MATERIAL);

        materials.resize(MAX_MATERIAL);
        for (std::size_t i = 0; i < MAX_MATERIAL; ++i) {
            if (materialFilenames.size() > i && !materialFilenames[i].empty()) {
                materials[i] = ResourceRetriever::instance().getResource<Material>(materialFilenames[i], {});
            } else {
                materials[i] = std::shared_ptr<Material>(nullptr);
            }
        }
    }

    void TerrainMaterials::refreshWith(unsigned int xSize, unsigned int zSize) {
        buildTexCoordinates(xSize, zSize);
    }

    const std::string& TerrainMaterials::getMaskMapFilename() const {
        return maskMapFilename;
    }

    const std::shared_ptr<Texture>& TerrainMaterials::getMaskTexture() const {
        return maskTexture;
    }

    Vector2<float> TerrainMaterials::getStRepeat() const {
        return Vector2<float>(sRepeat, tRepeat);
    }

    const std::vector<std::shared_ptr<Material>>& TerrainMaterials::getMaterials() const {
        return materials;
    }

    void TerrainMaterials::buildTexCoordinates(unsigned int xSize, unsigned int zSize) {
        texCoordinates.reserve(xSize * zSize);

        for (unsigned int z = 0; z < zSize; ++z) {
            for (unsigned int x = 0; x < xSize; ++x) {
                float s = (float)x / (float)xSize * sRepeat;
                float t = (float)z / (float)zSize * tRepeat;

                texCoordinates.emplace_back(Point2<float>(s, t));
            }
        }
    }

    const std::vector<Point2<float>>& TerrainMaterials::getTexCoordinates() const {
        return texCoordinates;
    }
}
