#include "TerrainReaderWriter.h"

namespace urchin {
    Terrain* TerrainReaderWriter::loadFrom(const XmlChunk* terrainChunk, const DataParser& dataParser) const {
        Terrain* terrain = buildTerrainFrom(terrainChunk, dataParser);

        loadPropertiesOn(terrain, terrainChunk, dataParser);
        loadGrassOn(terrain, terrainChunk, dataParser);

        return terrain;
    }

    void TerrainReaderWriter::writeOn(XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        buildChunkFrom(terrainChunk, terrain, xmlWriter);

        writePropertiesOn(terrainChunk, terrain, xmlWriter);
        writeGrassOn(terrainChunk, terrain, xmlWriter);
    }

    Terrain* TerrainReaderWriter::buildTerrainFrom(const XmlChunk* terrainChunk, const DataParser& dataParser) const {
        auto meshChunk = dataParser.getUniqueChunk(true, MESH_TAG, DataAttribute(), terrainChunk);
        auto heightFilenameChunk = dataParser.getUniqueChunk(true, HEIGHT_FILENAME_TAG, DataAttribute(), meshChunk.get());
        auto xzScaleChunk = dataParser.getUniqueChunk(true, XZ_SCALE_TAG, DataAttribute(), meshChunk.get());
        auto yScaleChunk = dataParser.getUniqueChunk(true, Y_SCALE_TAG, DataAttribute(), meshChunk.get());
        auto terrainMesh = std::make_shared<TerrainMesh>(heightFilenameChunk->getStringValue(), xzScaleChunk->getFloatValue(), yScaleChunk->getFloatValue());

        auto materialChunk = dataParser.getUniqueChunk(true, MATERIAL_TAG, DataAttribute(), terrainChunk);
        auto maskMapFilenameChunk = dataParser.getUniqueChunk(true, MASK_MAP_FILENAME, DataAttribute(), materialChunk.get());
        auto sRepeatChunk = dataParser.getUniqueChunk(true, S_REPEAT_TAG, DataAttribute(), materialChunk.get());
        auto tRepeatChunk = dataParser.getUniqueChunk(true, T_REPEAT_TAG, DataAttribute(), materialChunk.get());
        auto materialFilenamesChunk = dataParser.getUniqueChunk(true, MATERIAL_FILENAMES, DataAttribute(), materialChunk.get());
        std::vector<std::string> materialFilenames;
        for (unsigned int i = 0; i < TerrainMaterials::MAX_MATERIAL; ++i) {
            auto materialFilenameChunk = dataParser.getUniqueChunk(false, MATERIAL_FILENAME, DataAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk.get());
            if (materialFilenameChunk) {
                materialFilenames.emplace_back(materialFilenameChunk->getStringValue());
            } else {
                materialFilenames.emplace_back("");
            }
        }
        auto terrainMaterial = std::make_unique<TerrainMaterials>(maskMapFilenameChunk->getStringValue(), materialFilenames, sRepeatChunk->getFloatValue(), tRepeatChunk->getFloatValue());

        auto positionChunk = dataParser.getUniqueChunk(true, POSITION_TAG, DataAttribute(), terrainChunk);

        return new Terrain(terrainMesh, std::move(terrainMaterial), positionChunk->getPoint3Value());
    }

    void TerrainReaderWriter::buildChunkFrom(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        auto meshChunk = xmlWriter.createChunk(MESH_TAG, DataAttribute(), terrainChunk);
        auto heightFilenameChunk = xmlWriter.createChunk(HEIGHT_FILENAME_TAG, DataAttribute(), meshChunk.get());
        heightFilenameChunk->setStringValue(terrain->getMesh()->getHeightFilename());
        auto xzScaleChunk = xmlWriter.createChunk(XZ_SCALE_TAG, DataAttribute(), meshChunk.get());
        xzScaleChunk->setFloatValue(terrain->getMesh()->getXZScale());
        auto yScaleChunk = xmlWriter.createChunk(Y_SCALE_TAG, DataAttribute(), meshChunk.get());
        yScaleChunk->setFloatValue(terrain->getMesh()->getYScale());

        auto materialChunk = xmlWriter.createChunk(MATERIAL_TAG, DataAttribute(), terrainChunk);
        auto maskMapFilenameChunk = xmlWriter.createChunk(MASK_MAP_FILENAME, DataAttribute(), materialChunk.get());
        maskMapFilenameChunk->setStringValue(terrain->getMaterials()->getMaskMapFilename());
        auto sRepeatChunk = xmlWriter.createChunk(S_REPEAT_TAG, DataAttribute(), materialChunk.get());
        sRepeatChunk->setFloatValue(terrain->getMaterials()->getStRepeat().X);
        auto tRepeatChunk = xmlWriter.createChunk(T_REPEAT_TAG, DataAttribute(), materialChunk.get());
        tRepeatChunk->setFloatValue(terrain->getMaterials()->getStRepeat().Y);
        auto materialFilenamesChunk = xmlWriter.createChunk(MATERIAL_FILENAMES, DataAttribute(), materialChunk.get());
        unsigned int i = 0;
        for (const Material* material : terrain->getMaterials()->getMaterials()) {
            if (material != nullptr) {
                auto materialFilenameChunk = xmlWriter.createChunk(MATERIAL_FILENAME, DataAttribute(INDEX_ATTR, std::to_string(i)), materialFilenamesChunk.get());
                materialFilenameChunk->setStringValue(material->getName());
            }
            ++i;
        }

        auto positionChunk = xmlWriter.createChunk(POSITION_TAG, DataAttribute(), terrainChunk);
        positionChunk->setPoint3Value(terrain->getPosition());
    }

    void TerrainReaderWriter::loadPropertiesOn(Terrain* terrain, const XmlChunk* terrainChunk, const DataParser& dataParser) const {
        auto ambientChunk = dataParser.getUniqueChunk(true, AMBIENT_TAG, DataAttribute(), terrainChunk);
        terrain->setAmbient(ambientChunk->getFloatValue());
    }

    void TerrainReaderWriter::writePropertiesOn(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        auto ambientChunk = xmlWriter.createChunk(AMBIENT_TAG, DataAttribute(), terrainChunk);
        ambientChunk->setFloatValue(terrain->getAmbient());
    }

    void TerrainReaderWriter::loadGrassOn(Terrain* terrain, const XmlChunk* terrainChunk, const DataParser& dataParser) const {
        auto grassChunk = dataParser.getUniqueChunk(false, GRASS_TAG, DataAttribute(), terrainChunk);
        if (grassChunk) {
            auto grassTextureFilenameChunk = dataParser.getUniqueChunk(true, GRASS_TEXTURE_FILENAME_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassTexture(grassTextureFilenameChunk->getStringValue());

            auto grassMaskFilenameChunk = dataParser.getUniqueChunk(true, GRASS_MASK_FILENAME_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setMaskTexture(grassMaskFilenameChunk->getStringValue());

            auto numGrassInTexChunk = dataParser.getUniqueChunk(true, NUM_GRASS_IN_TEX_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setNumGrassInTexture(numGrassInTexChunk->getUnsignedIntValue());

            auto grassHeightChunk = dataParser.getUniqueChunk(true, GRASS_HEIGHT_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassHeight(grassHeightChunk->getFloatValue());

            auto grassLengthChunk = dataParser.getUniqueChunk(true, GRASS_LENGTH_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassLength(grassLengthChunk->getFloatValue());

            auto grassQuantityChunk = dataParser.getUniqueChunk(true, GRASS_QUANTITY_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setGrassQuantity(grassQuantityChunk->getFloatValue());

            auto windDirectionChunk = dataParser.getUniqueChunk(true, WIND_DIRECTION_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setWindDirection(windDirectionChunk->getVector3Value());

            auto windStrengthChunk = dataParser.getUniqueChunk(true, WIND_STRENGTH_TAG, DataAttribute(), grassChunk.get());
            terrain->getGrass()->setWindStrength(windStrengthChunk->getFloatValue());
        }
    }

    void TerrainReaderWriter::writeGrassOn(const XmlChunk* terrainChunk, const Terrain* terrain, XmlWriter& xmlWriter) const {
        if (terrain->getGrass()) {
            auto grassChunk = xmlWriter.createChunk(GRASS_TAG, DataAttribute(), terrainChunk);

            auto grassTextureFilenameChunk = xmlWriter.createChunk(GRASS_TEXTURE_FILENAME_TAG, DataAttribute(), grassChunk.get());
            grassTextureFilenameChunk->setStringValue(terrain->getGrass()->getGrassTexture());

            auto grassMaskFilenameChunk = xmlWriter.createChunk(GRASS_MASK_FILENAME_TAG, DataAttribute(), grassChunk.get());
            grassMaskFilenameChunk->setStringValue(terrain->getGrass()->getMaskTexture());

            auto numGrassInTexChunk = xmlWriter.createChunk(NUM_GRASS_IN_TEX_TAG, DataAttribute(), grassChunk.get());
            numGrassInTexChunk->setUnsignedIntValue(terrain->getGrass()->getNumGrassInTexture());

            auto grassHeightChunk = xmlWriter.createChunk(GRASS_HEIGHT_TAG, DataAttribute(), grassChunk.get());
            grassHeightChunk->setFloatValue(terrain->getGrass()->getGrassHeight());

            auto grassLengthChunk = xmlWriter.createChunk(GRASS_LENGTH_TAG, DataAttribute(), grassChunk.get());
            grassLengthChunk->setFloatValue(terrain->getGrass()->getGrassLength());

            auto grassQuantityChunk = xmlWriter.createChunk(GRASS_QUANTITY_TAG, DataAttribute(), grassChunk.get());
            grassQuantityChunk->setFloatValue(terrain->getGrass()->getGrassQuantity());

            auto windDirectionChunk = xmlWriter.createChunk(WIND_DIRECTION_TAG, DataAttribute(), grassChunk.get());
            windDirectionChunk->setVector3Value(terrain->getGrass()->getWindDirection());

            auto windStrengthChunk = xmlWriter.createChunk(WIND_STRENGTH_TAG, DataAttribute(), grassChunk.get());
            windStrengthChunk->setFloatValue(terrain->getGrass()->getWindStrength());
        }
    }
}
