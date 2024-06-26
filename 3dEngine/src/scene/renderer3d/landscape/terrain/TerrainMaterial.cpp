#include <stdexcept>

#include <scene/renderer3d/landscape/terrain/TerrainMaterial.h>
#include <resources/ResourceRetriever.h>

namespace urchin {

    TerrainMaterials::TerrainMaterials(std::string maskMapFilename, const std::vector<std::string>& materialFilenames, float sRepeat, float tRepeat) :
            sRepeat(sRepeat),
            tRepeat(tRepeat) {
        if (maskMapFilename.empty() || FileUtil::isAbsolutePath(maskMapFilename)) {
            this->maskMapFilename = std::move(maskMapFilename);
        } else {
            this->maskMapFilename = FileSystem::instance().getResourcesDirectory() + std::move(maskMapFilename);
        }

        if (this->maskMapFilename.empty()) {
            std::vector<unsigned char> terrainMaskColor({255, 0, 0, 0});
            maskTexture = Texture::build("default terrain mask", 1, 1, TextureFormat::RGBA_8_INT, terrainMaskColor.data(), TextureDataType::INT_8);
        } else {
            maskTexture = ResourceRetriever::instance().getResource<Texture>(this->maskMapFilename, {{"mipMap", "0"}});
            if (maskTexture->getFormat() != TextureFormat::RGBA_8_INT) {
                throw std::runtime_error("Mask texture must have 4 components (RGBA). Texture format: " + std::to_string((int)maskTexture->getFormat()));
            }
        }

        initializeMaterial(materialFilenames);
    }

    void TerrainMaterials::initializeMaterial(const std::vector<std::string>& materialFilenames) {
        assert(materialFilenames.size() <= MAX_MATERIAL);

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
        return {sRepeat, tRepeat};
    }

    const std::array<std::shared_ptr<Material>, TerrainMaterials::MAX_MATERIAL>& TerrainMaterials::getMaterials() const {
        return materials;
    }

    void TerrainMaterials::buildTexCoordinates(unsigned int xSize, unsigned int zSize) {
        texCoordinates.reserve(xSize * zSize);

        for (unsigned int z = 0; z < zSize; ++z) {
            for (unsigned int x = 0; x < xSize; ++x) {
                float s = (float)x / (float)xSize * sRepeat;
                float t = (float)z / (float)zSize * tRepeat;

                texCoordinates.emplace_back(s, t);
            }
        }
    }

    const std::vector<Point2<float>>& TerrainMaterials::getTexCoordinates() const {
        return texCoordinates;
    }
}
