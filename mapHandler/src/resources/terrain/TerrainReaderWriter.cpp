#include "TerrainReaderWriter.h"

namespace urchin {
    Terrain* TerrainReaderWriter::loadFrom(const std::shared_ptr<XmlChunk>& terrainChunk, const XmlParser& xmlParser) const {
        Terrain* terrain = buildTerrainFrom(terrainChunk, xmlParser);

        loadPropertiesOn(terrain, terrainChunk, xmlParser);
        loadGrassOn(terrain, terrainChunk, xmlParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(const std::shared_ptr<XmlChunk>& terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        buildChunkFrom(terrainChunk, terrain, xmlWriter);

        writePropertiesOn(terrainChunk, terrain, xmlWriter);
        writeGrassOn(terrainChunk, terrain, xmlWriter);
    }

    Terrain* TerrainReaderWriter::buildTerrainFrom(const std::shared_ptr<XmlChunk>& terrainChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> meshChunk = xmlParser.getUniqueChunk(true, MESH_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> heightFilenameChunk = xmlParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk);
        std::shared_ptr<XmlChunk> xzScaleChunk = xmlParser.getUniqueChunk(true, XZ_SCALE_TAG, XmlAttribute(), meshChunk);
        std::shared_ptr<XmlChunk> yScaleChunk = xmlParser.getUniqueChunk(true, Y_SCALE_TAG, XmlAttribute(), meshChunk);
        auto terrainMesh = std::make_shared<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        std::shared_ptr<XmlChunk> materialChunk = xmlParser.getUniqueChunk(true, MATERIAL_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> maskMapFilenameChunk = xmlParser.getUniqueChunk(true, MASK_MAP_FILENAME, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> sRepeatChunk = xmlParser.getUniqueChunk(true, S_REPEAT_TAG, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> tRepeatChunk = xmlParser.getUniqueChunk(true, T_REPEAT_TAG, XmlAttribute(), materialChunk);
        std::shared_ptr<XmlChunk> materialFilenamesChunk = xmlParser.getUniqueChunk(true, MATERIAL_FILENAMES, XmlAttribute(), materialChunk);
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < MAX_MATERIAL; ++i) {
            std::shared_ptr<XmlChunk> materialFilenameChunk = xmlParser.getUniqueChunk(false, MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterial>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        std::shared_ptr<XmlChunk> positionChunk = xmlParser.getUniqueChunk(true, POSITION_TAG, XmlAttribute(), terrainChunk);

        return new Terrain(terrainMesh, terrainMaterial, positionChunk->getPoint3Value());
    }

    void TerrainReaderWriter::buildChunkFrom(const std::shared_ptr<XmlChunk>& terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        std::shared_ptr<XmlChunk> meshChunk = xmlWriter.createChunk(MESH_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> heightFilenameChunk = xmlWriter.createChunk(HEIGHT_FILENAME_TAG, XmlAttribute(), meshChunk);
        heightFilenameChunk->setStringValue(terrain->getMesh()->getHeightFilename());
        std::shared_ptr<XmlChunk> xzScaleChunk = xmlWriter.createChunk(XZ_SCALE_TAG, XmlAttribute(), meshChunk);
        xzScaleChunk->setFloatValue(terrain->getMesh()->getXZScale());
        std::shared_ptr<XmlChunk> yScaleChunk = xmlWriter.createChunk(Y_SCALE_TAG, XmlAttribute(), meshChunk);
        yScaleChunk->setFloatValue(terrain->getMesh()->getYScale());

        std::shared_ptr<XmlChunk> materialChunk = xmlWriter.createChunk(MATERIAL_TAG, XmlAttribute(), terrainChunk);
        std::shared_ptr<XmlChunk> maskMapFilenameChunk = xmlWriter.createChunk(MASK_MAP_FILENAME, XmlAttribute(), materialChunk);
        maskMapFilenameChunk->setStringValue(terrain->getMaterial()->getMaskMapFilename());
        std::shared_ptr<XmlChunk> sRepeatChunk = xmlWriter.createChunk(S_REPEAT_TAG, XmlAttribute(), materialChunk);
        sRepeatChunk->setFloatValue(terrain->getMaterial()->getSRepeat());
        std::shared_ptr<XmlChunk> tRepeatChunk = xmlWriter.createChunk(T_REPEAT_TAG, XmlAttribute(), materialChunk);
        tRepeatChunk->setFloatValue(terrain->getMaterial()->getTRepeat());
        std::shared_ptr<XmlChunk> materialFilenamesChunk = xmlWriter.createChunk(MATERIAL_FILENAMES, XmlAttribute(), materialChunk);
        unsigned int i=0;
        for (const Material* material : terrain->getMaterial()->getMaterials()) {
            if (material != nullptr) {
                std::shared_ptr<XmlChunk> materialFilenameChunk = xmlWriter.createChunk(MATERIAL_FILENAME, XmlAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk);
                materialFilenameChunk->setStringValue(material->getName());
            }
            ++i;
        }

        std::shared_ptr<XmlChunk> positionChunk = xmlWriter.createChunk(POSITION_TAG, XmlAttribute(), terrainChunk);
        positionChunk->setPoint3Value(terrain->getPosition());
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain* terrain, const std::shared_ptr<XmlChunk>& terrainChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> ambientChunk = xmlParser.getUniqueChunk(true, AMBIENT_TAG, XmlAttribute(), terrainChunk);
        terrain->setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(const std::shared_ptr<XmlChunk>& terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        std::shared_ptr<XmlChunk> ambientChunk = xmlWriter.createChunk(AMBIENT_TAG, XmlAttribute(), terrainChunk);
        ambientChunk->setFloatValue(terrain->getAmbient());
    }

    void TerrainReaderWriter::loadGrassOn(Terrain* terrain, const std::shared_ptr<XmlChunk>& terrainChunk, const XmlParser& xmlParser) const {
        std::shared_ptr<XmlChunk> grassChunk = xmlParser.getUniqueChunk(false, GRASS_TAG, XmlAttribute(), terrainChunk);
        if (grassChunk) {
            std::shared_ptr<XmlChunk> grassTextureFilenameChunk = xmlParser.getUniqueChunk(true, GRASS_TEXTURE_FILENAME_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setGrassTexture(grassTextureFilenameChunk->getStringValue());

            std::shared_ptr<XmlChunk> grassMaskFilenameChunk = xmlParser.getUniqueChunk(true, GRASS_MASK_FILENAME_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setMaskTexture(grassMaskFilenameChunk->getStringValue());

            std::shared_ptr<XmlChunk> numGrassInTexChunk = xmlParser.getUniqueChunk(true, NUM_GRASS_IN_TEX_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            std::shared_ptr<XmlChunk> grassHeightChunk = xmlParser.getUniqueChunk(true, GRASS_HEIGHT_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setGrassHeight(grassHeightChunk->getFloatValue());

            std::shared_ptr<XmlChunk> grassLengthChunk = xmlParser.getUniqueChunk(true, GRASS_LENGTH_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setGrassLength(grassLengthChunk->getFloatValue());

            std::shared_ptr<XmlChunk> grassQuantityChunk = xmlParser.getUniqueChunk(true, GRASS_QUANTITY_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setGrassQuantity(grassQuantityChunk->getFloatValue());

            std::shared_ptr<XmlChunk> windDirectionChunk = xmlParser.getUniqueChunk(true, WIND_DIRECTION_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setWindDirection(windDirectionChunk->getVector3Value());

            std::shared_ptr<XmlChunk> windStrengthChunk = xmlParser.getUniqueChunk(true, WIND_STRENGTH_TAG, XmlAttribute(), grassChunk);
            terrain->getGrass()->setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainReaderWriter::writeGrassOn(const std::shared_ptr<XmlChunk>& terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        if (terrain->getGrass()) {
            std::shared_ptr<XmlChunk> grassChunk = xmlWriter.createChunk(GRASS_TAG, XmlAttribute(), terrainChunk);

            std::shared_ptr<XmlChunk> grassTextureFilenameChunk = xmlWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, XmlAttribute(), grassChunk);
            grassTextureFilenameChunk->setStringValue(terrain->getGrass()->getGrassTexture());

            std::shared_ptr<XmlChunk> grassMaskFilenameChunk = xmlWriter.createChunk(GRASS_MASK_FILENAME_TAG, XmlAttribute(), grassChunk);
            grassMaskFilenameChunk->setStringValue(terrain->getGrass()->getMaskTexture());

            std::shared_ptr<XmlChunk> numGrassInTexChunk = xmlWriter.createChunk(NUM_GRASS_IN_TEX_TAG, XmlAttribute(), grassChunk);
            numGrassInTexChunk->setUnsignedIntValue(terrain->getGrass()->getNumGrassInTexture());

            std::shared_ptr<XmlChunk> grassHeightChunk = xmlWriter.createChunk(GRASS_HEIGHT_TAG, XmlAttribute(), grassChunk);
            grassHeightChunk->setFloatValue(terrain->getGrass()->getGrassHeight());

            std::shared_ptr<XmlChunk> grassLengthChunk = xmlWriter.createChunk(GRASS_LENGTH_TAG, XmlAttribute(), grassChunk);
            grassLengthChunk->setFloatValue(terrain->getGrass()->getGrassLength());

            std::shared_ptr<XmlChunk> grassQuantityChunk = xmlWriter.createChunk(GRASS_QUANTITY_TAG, XmlAttribute(), grassChunk);
            grassQuantityChunk->setFloatValue(terrain->getGrass()->getGrassQuantity());

            std::shared_ptr<XmlChunk> windDirectionChunk = xmlWriter.createChunk(WIND_DIRECTION_TAG, XmlAttribute(), grassChunk);
            windDirectionChunk->setVector3Value(terrain->getGrass()->getWindDirection());

            std::shared_ptr<XmlChunk> windStrengthChunk = xmlWriter.createChunk(WIND_STRENGTH_TAG, XmlAttribute(), grassChunk);
            windStrengthChunk->setFloatValue(terrain->getGrass()->getWindStrength());
        }
    }
}
